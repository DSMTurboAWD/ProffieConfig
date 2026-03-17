/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2024-2026 Ryan Ogurek
 *
 * launcher/main.cpp
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <wx/app.h>
#include <wx/utils.h>
#include <wx/progdlg.h>
#include <wx/snglinst.h>
#include <wx/msgdlg.h>

#include "app/app.hpp"
#include "app/critical_dialog.hpp"
#include "log/context.hpp"
#include "log/logger.hpp"
#include "utils/paths.hpp"
#include "utils/defer.hpp"
#include "ui/dialogs/message.hpp"

#include "routines.hpp"
#include "update/update.hpp"
#include "update/pulldata.hpp"
#include "update/changelog.hpp"
#include "update/install.hpp"

class Launcher : public wxApp {
public:
    bool OnInit() override {
        app::setName("ProffieConfig Launcher");
        app::provideUI(pcui::showMessage);

        if (auto ec{paths::init()}) {
            pcui::showMessage(wxString::Format(
                _("Could not setup paths: %s"),
                ec.message()
            ));
            return false;
        }

        if (not app::setupExclusion("ProffieConfig")) {
            return false;
        }

        if (not app::init()) {
            app::CriticalDialog dlg{_("Initialization Failed")};
            return false;
        }

        auto& logger{logging::Context::getGlobal().createLogger("Launcher")};

        logger.info("Checking installation status...");
        auto currentExec{paths::executable()};
        auto installedExec{paths::executable(paths::Executable::Launcher)};
        std::error_code err;
        if (not fs::equivalent(currentExec, installedExec, err)) {
            routine::platformInstall(*logger.binfo("Launcher not installed, running install sequence..."));
            return false;
        }
        logger.info("Launcher installed, continuing...");

        enum class Action {
            LAUNCH,
            FIRST_INSTALL,
            UNINSTALL
        } action{Action::LAUNCH};
        
        if (argc == 2 and argv[1] == "uninstall") {
            action = Action::UNINSTALL;
            if (wxNO == pcui::showMessage(_("Are you sure you want to uninstall ProffieConfig?"), app::getName(), wxYES_NO | wxNO_DEFAULT)) {
                return false;
            }
        } else {
            if (not fs::exists(paths::executable(paths::Executable::Main))) {
                action = Action::FIRST_INSTALL;
                logger.info("Main ProffieConfig binary missing, update/install routine required.");
                if (wxNO == pcui::showMessage(_("ProffieConfig installation needs to run, continue?"), app::getName(), wxYES_NO | wxYES_DEFAULT)) {
                    return false;
                }
            }
        }

        wxString statusStr{};
        switch (action) {
            case Action::LAUNCH:
                statusStr = "Update Check";
                break;
            case Action::FIRST_INSTALL:
                statusStr = "First Install";
                break;
            case Action::UNINSTALL:
                statusStr = "Uninstall";
                break;
        }
        pcui::ProgressDialog prog(
            nullptr,
            "ProffieConfig Launcher | " + statusStr,
            true,
            {300, -1}
        );

        // Push full out of range.
        // TODO: This really should be changed so that each download fills
        // a respective portion of the progress...
        // If there is to be a progress bar for the current download, it should
        // be in conjunction with the overall progress.
        prog.range(101);

        Update::init();

        if (action == Action::LAUNCH or action == Action::FIRST_INSTALL) {
            auto pullSuccess{Update::pullData(&prog, *logger.binfo("Collecting version data..."))};
            if (not pullSuccess) {
                logger.info("Aborting update after failed version data collection...");
                if (action == Action::FIRST_INSTALL) {
                    prog.finish(true, _("Failed pulling update data, please try again!"));
                    return false;
                }

                routine::launch(*logger.binfo("Launching without update..."));
                return false;
            }

            auto data{Update::parseData(&prog, *logger.binfo("Parsing version data..."))};
            if (not data) {
                prog.finish(true, _("Failed to parse data!\nPlease report this error."));
                wxLaunchDefaultApplication(paths::logDir().native());
                return false;
            }

            if (data->bundles.empty()) {
                logger.error("No valid bundles found!");
                prog.finish(true, _("No valid version bundles found!\nPlease report this error."));
                wxLaunchDefaultApplication(paths::logDir().native());
                if (action == Action::LAUNCH) routine::launch(*logger.binfo("Launching in lieu of valid update data."));
                return false;
            }

            utils::Version currentVersion;
            currentVersion = Update::determineCurrentVersion(data.value(), &prog, *logger.binfo("Determining current version."));
            auto changelog{Update::generateChangelog(data.value(), currentVersion, *logger.binfo("Generating changelog..."))};
            logger.info("Current Version: " + changelog.currentBundleVersion.string());
            logger.info("Latest Version: " + changelog.latestBundleVersion.string());

            prog.hide();

            if (
                    changelog.currentBundleVersion and
                    changelog.latestBundleVersion.compare(changelog.currentBundleVersion) == 0
                ) {
                routine::launch(*logger.binfo("Up to date, launching..."));
                return false;
            }

            if (
                    action == Action::LAUNCH and
                    not Update::promptWithChangelog(data.value(), changelog, *logger.binfo("Prompting user for update..."))
               ) {
                routine::launch(*logger.binfo("User declined update."));
                return false;
            }
            prog.show();

            if (not Update::pullNewFiles(changelog, data.value(), &prog, *logger.binfo("Downloading new files..."))) {
                prog.finish(false);

                logger.info("Aborting update after failed download.");

                if (action == Action::LAUNCH) routine::launch(*logger.binfo("Launching..."));
                return false;
            }

            Update::installFiles(changelog, data.value(), &prog, *logger.binfo("Installing files..."));

            prog.pulse();
            wxYield();
            prog.finish(false);

            if (action == Action::FIRST_INSTALL) {
                pcui::showMessage(_("Installed"), app::getName());
            }
        } else if (action == Action::UNINSTALL) {
            logging::Context::destroyGlobal();

            std::error_code err;
            prog.set(30, "Removing binaries...");
            fs::remove_all(paths::binaryDir(), err);
            prog.set(40, "Removing libraries...");
            fs::remove_all(paths::libraryDir(), err);
            prog.set(50, "Removing components...");
            fs::remove_all(paths::componentDir(), err);
            prog.set(60, "Removing resources...");
            fs::remove_all(paths::resourceDir(), err);

            if (wxYES == pcui::showMessage(_("Purge user data? (configurations, saves, etc.)\nIf files are kept, they will be available if reinstalled."), app::getName(), wxYES_NO | wxNO_DEFAULT)) {
                prog.set(70, "Purging user data...");
                fs::remove_all(paths::dataDir());
            }

            prog.set(90, "Removing platform setup...");
            routine::platformUninstall();

            prog.set(95, "Purging logs...");
            fs::remove_all(paths::logDir(), err);

            prog.set(95, "Finalizing...");
#           ifdef __APPLE__
            const auto currentBundle{currentExec.parent_path().parent_path().parent_path()};
            fs::remove_all(currentBundle);
#           elif defined (_WIN32)
            MoveFileExW(currentExec.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
#           elif defined (__linux__)
            (void)remove(currentExec.c_str());
#           endif

            prog.set(101, "Uninstalled.");
            return false;
        }

        routine::launch(*logger.binfo("Launcher routines done."));
        return false;
    }
};

wxIMPLEMENT_APP(Launcher);

