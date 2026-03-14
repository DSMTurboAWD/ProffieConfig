#include "mainmenu.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2024-2026 Ryan Ogurek
 *
 * proffieconfig/mainmenu/mainmenu.cpp
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

#include <wx/aboutdlg.h>
#include <wx/cursor.h>
#include <wx/collpane.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/toplevel.h>
#include <wx/utils.h>

#include "app/info.hpp"
#include "config/config.hpp"
#include "data/logic/adapter.hpp"
#include "data/logic/operators.hpp"
#include "ui/controls/button.hpp"
#include "ui/controls/choice.hpp"
#include "ui/controls/text.hpp"
#include "ui/helpers/foreach.hpp"
#include "ui/layout/collapsible.hpp"
#include "ui/layout/scrolled.hpp"
#include "ui/layout/spacer.hpp"
#include "ui/layout/stack.hpp"
#include "ui/misc/message.hpp"
#include "ui/frame.hpp"
#include "ui/dialog.hpp"
#include "ui/static/image.hpp"
#include "ui/static/label.hpp"
#include "utils/defer.hpp"
#include "utils/paths.hpp"
#include "utils/string.hpp"
#include "dialogs/addconfig.hpp"
#include "dialogs/manifest.hpp"
#include "config/info.hpp"
#include "log/info.hpp"
#include "pconf/info.hpp"
#include "ui/info.hpp"
#include "utils/info.hpp"
#include "versions/info.hpp"
// #include "versions_manager/info.h"
// #include "versions_manager/manager.h"

#include "../core/state.hpp"
#include "../core/licenses.hpp"
// #include "../core/appstate.h"
#include "../core/utilities/misc.h"
// #include "../core/utilities/progress.h"
// #include "../editor/editorwindow.h"
// #include "../tools/arduino.h"
// #include "../tools/serialmonitor.h"
#include "../onboard/onboard.hpp"
#include "wx/font.h"

MainMenu *MainMenu::instance{nullptr};

MainMenu::MainMenu(wxWindow* parent) : 
    pcui::Frame(
        parent,
        state::eID_Main_Menu,
        "ProffieConfig",
        wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)
    ) {

    // REVIEW
    // boardSelection.setPersistence(pcui::ChoiceData::Persistence::String);
    // configSelection.setPersistence(pcui::ChoiceData::Persistence::String);

    createMenuBar();
    bindEvents();

    pcui::build(this, ui());

    Show();
}

MainMenu::~MainMenu() {
    pcui::teardown(this);
}

void MainMenu::bindEvents() {
    auto promptClose{[this]() -> bool {
        for (auto *editor : mEditors) {
            // REVIEW
            // if (not editor->getOpenConfig().isSaved()) {
            //     auto res{pcui::showMessage(
            //         _("There is at least one editor open with unsaved changes, are you sure you want to exit?") +
            //         "\n\n"+
            //         _("All unsaved changes will be lost!"),
            //         _("Open Editor(s)"),
            //         wxYES_NO | wxNO_DEFAULT | wxCENTER | wxICON_EXCLAMATION
            //     )};
            //     if (res != wxYES) return false;
            //     break;
            // }
        }

        return true;
    }};

    Bind(wxEVT_CLOSE_WINDOW, [promptClose](wxCloseEvent& event) {
        state::saveState();

        if (event.CanVeto() and not promptClose()) {
            event.Veto();
            return;
        }
        event.Skip();
    });
    Bind(wxEVT_MENU, [&](wxCommandEvent&) { Close(true); }, wxID_EXIT);
    Bind(wxEVT_MENU, [&](wxCommandEvent&) {
        wxAboutDialogInfo aboutInfo;
        const auto componentVersions{
            std::string{} +
            "App: v" + app::version() + "\n"
            "Config: v" + config::version() + "\n"
            "Log: v" + logging::version() + "\n"
            "PConf: v" + pconf::version() + "\n"
            "pcui: v" + pcui::version() + "\n"
            "Utils: v" + utils::version() + "\n"
            "Versions: v" + versions::version() + "\n"
            // REVIEW
            // "Versions Manager: v" + versions_manager::version() + "\n"
            // "Arduino CLI: v" + arduino::version() + "\n"
            //
        };
#       ifdef __WXOSX__
        aboutInfo.SetDescription(_("All-in-one Proffieboard Management Utility"));
        aboutInfo.SetVersion(
            wxSTRINGIZE(BIN_VERSION),
            "Core: v" + wxString{wxSTRINGIZE(BIN_VERSION)} + "\n"
            + componentVersions
        );
#       else
        aboutInfo.SetDescription(
            _("All-in-one Proffieboard Management Utility") + "\n\n"
            + componentVersions
        );
        aboutInfo.SetVersion(wxSTRINGIZE(BIN_VERSION));
#       endif
#       ifdef __WXGTK__
        aboutInfo.SetWebSite("https://proffieconfig.kafrenetrading.com");
#       endif
        aboutInfo.SetCopyright("Copyright (C) 2023-2026 Ryan Ogurek");
        aboutInfo.SetName("ProffieConfig");
        wxAboutBox(aboutInfo, this);
    }, wxID_ABOUT);

    Bind(wxEVT_MENU, [&](wxCommandEvent &) {
        wxLaunchDefaultApplication(paths::logDir().native());
    }, eID_Logs);

    Bind(wxEVT_MENU, [&](wxCommandEvent &) {
        pcui::Dialog dialog{
            this,
            wxID_ANY,
            wxString::Format(_("ProffieConfig Copyright & License Info")),
            wxDEFAULT_DIALOG_STYLE | wxCENTER | wxRESIZE_BORDER
        };

        auto licenseFont{pcui::text::detail::StyleData{}.makeFont()};
        licenseFont.SetFamily(wxFONTFAMILY_TELETYPE);

        const auto ui{pcui::Scrolled{
          .win_={
            .base_={
              .minSize_={-1, 600},
              .expand_=true,
              .proportion_=1,
            },
          },
          .scrollRate_={.y_=10},
          .child_=pcui::Stack{
            .base_={
                .expand_=true,
                .border_={.size_=10, .dirs_=wxLEFT | wxRIGHT},
            },
            .children_={
              pcui::ForEach{
                .of_=LICENSES,
                .do_=[&](const LicenseInfo& info) -> pcui::DescriptorPtr {
                    return pcui::Stack{
                      .base_={.expand_=true},
                      .children_={
                        pcui::Spacer{.size_=10}(),
                        pcui::Label{
                          .label_=info.name_,
                          .style_=pcui::text::Style::Header,
                        }(),
                        pcui::Label{
                          .label_=wxString{L"Copyright \u00A9 "} +
                              info.date_ + ' ' + info.author_
                        }(),
                        pcui::Label{
                          .label_=info.detail_,
                        }(),
                        pcui::Collapsible{
                          .win_={.base_={.expand_=true}},
                          .showLabel_="Show License",
                          .hideLabel_="Hide License",
                          .child_=pcui::Text{
                            .win_={
                              .base_={
                                .minSize_={
                                  [&] {
                                      int x{};
                                      int y{};
                                      GetTextExtent(
                                          'M',
                                          &x, &y,
                                          nullptr,
                                          nullptr,
                                          &licenseFont
                                      );
                                      return x * 80;
                                  }(),
                                  -1
                                },
                                .expand_=true,
                              },
                            },
                            .data_=info.license_,
                            .autoLink_=true,
                            .style_=licenseFont,
                            .mode_=pcui::Text::MultiLine{
                              .scroll_={.vertical_=false, .horizontal_=false},
                            }
                          }(),
                        }(),
                      }
                    }();
                }
              }(),
              pcui::Spacer{.size_=10}(),
            },
          }(),
        }()};

        pcui::build(&dialog, ui);

        dialog.ShowModal();
    }, eID_Licenses);

    Bind(wxEVT_MENU, [&](wxCommandEvent &) {
        const auto warnPref{state::getPreference(
            state::ePreference_Hide_Editor_Manage_Versions_Warn
        )};
        if (
                not mEditors.empty() and
                not warnPref
            ) {
            auto res{pcui::showHideablePrompt(
                _("Although version management can be done with editors open, some information may be lost when adding/removing props."),
                _("Please Close Editors"),
                this,
                wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER,
                wxEmptyString,
                wxEmptyString,
                _("Proceed")
            )};
            state::setPreference(
                state::ePreference_Hide_Editor_Manage_Versions_Warn,
                res.wantsToHide_
            );
            if (res.result_ != wxID_OK) return;
        }

        // REVIEW
        // versions_manager::open(this, state::eID_Versions_Manager);
    }, eID_Manage_Versions);

    Bind(wxEVT_MENU, [&](wxCommandEvent &) {
        ManifestDialog(this).ShowModal();
    }, eID_Update_Manifest);

    Bind(wxEVT_MENU, [&](wxCommandEvent&) {
        wxLaunchDefaultBrowser("https://github.com/ryancog/ProffieConfig/blob/master/docs");
    }, eID_Docs);
    Bind(wxEVT_MENU, [&](wxCommandEvent&) {
        wxLaunchDefaultBrowser("https://github.com/ryancog/ProffieConfig/issues/new");
    }, eID_Issue);
    Bind(wxEVT_MENU, [&](wxCommandEvent&) {
        if (Close()) {
            onboard::Frame::instance = new onboard::Frame;
        }
    }, eID_Run_Setup);

    /*
    Bind(wxEVT_BUTTON, [&](wxCommandEvent&) {
        mNotifyData.notify(ID_AsyncStart);

        auto *progDialog{new Progress(this)};
        progDialog->SetTitle(_("Board Refresh"));
        progDialog->Update(0, _("Initializing..."));

        std::thread{[this, progDialog]() {
            progDialog->emitEvent(10, _("Discovering Boards..."));
            const auto boards{Arduino::getBoards()};

            progDialog->emitEvent(90, _("Processing and Finalizing..."));
            auto choices{boardSelection.choices()};
#           ifdef __WXOSX__
            if (choices.size() > 1) choices.erase(std::next(choices.begin()));
            choices.reserve(boards.size() + 1);
#           else
            choices.erase(std::next(choices.begin()), std::prev(choices.end()));
            choices.reserve(boards.size() + 2);
#           endif
            choices.insert(
                std::next(choices.begin()),
                boards.begin(),
                boards.end()
            );
            boardSelection.setChoices(std::move(choices));

            progDialog->emitEvent(100, _("Done"));
            mNotifyData.notify(ID_AsyncDone);
        }}.detach();
    }, eID_Refresh_Dev);

    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        mNotifyData.notify(ID_AsyncStart);

        auto *progDialog{new Progress(this)};
        progDialog->SetTitle(_("Applying Changes"));
        progDialog->Update(0, _("Initializing..."));

        std::thread{[this, progDialog]() {
            Defer defer{[this]() { mNotifyData.notify(ID_AsyncDone); }};

            progDialog->Update(1, _("Opening Config..."));

            auto *config{Config::getIfOpen(configSelection)};
            const auto configWasOpen{static_cast<bool>(config)};
            if (not configWasOpen) {
                const auto res{Config::open(configSelection)};
                if (const auto *err = std::get_if<string>(&res)) {
                    progDialog->Update(100, _("Error"));
                    auto *evt{new misc::MessageBoxEvent(
                        misc::EVT_MSGBOX, 
                        wxID_ANY, 
                        *err, 
                        _("Cannot Apply Changes")
                    )};
                    wxQueueEvent(this, evt);
                    return;
                }
                config = std::get<Config::Config *>(res);
            }

            const auto res{
                Arduino::applyToBoard(boardSelection, *config, progDialog)
            };

            if (const auto *err = std::get_if<string>(&res)) {
                auto *evt{new misc::MessageBoxEvent(
                    misc::EVT_MSGBOX,
                    wxID_ANY,
                    *err,
                    _("Cannot Apply Changes")
                )};
                wxQueueEvent(this, evt);
                return;
            }
            const auto& result{std::get<Arduino::Result>(res)};

            wxString message{_("Changes Successfully Applied to ProffieBoard!")};
            if (result.total != -1) {
                message += "\n\n";
                message += wxString::Format(
                    wxGetTranslation(Arduino::Result::USAGE_MESSAGE),
                    result.percent(),
                    result.used,
                    result.total
                );
            } 

            auto *evt{new misc::MessageBoxEvent(
                misc::EVT_MSGBOX,
                wxID_ANY,
                message,
                _("Apply Changes to Board"),
                wxOK | wxICON_INFORMATION
            )};
            wxQueueEvent(this, evt);
        }}.detach();
    }, eID_Apply_Changes);
    */

    /*
    Bind(wxEVT_BUTTON, [&](wxCommandEvent&) { 
        if (not SerialMonitor::instance) {
            SerialMonitor::instance = new SerialMonitor(this, boardSelection);
        } else {
            SerialMonitor::instance->Show();
            SerialMonitor::instance->Raise();
        }
    }, eID_Open_Serial);
    */

    /*
    Bind(wxEVT_BUTTON, [&](wxCommandEvent&) {
        mNotifyData.notify(ID_AsyncStart);

        std::thread{[this]() {
            Defer defer{[&]() { mNotifyData.notify(ID_AsyncDone); }};

            auto res{Config::open(configSelection)};
            if (auto *ptr = std::get_if<string>(&res)) {
                pcui::showMessage(*ptr, _("Cannot Edit Config"));
                return;
            }

            auto *config{std::get<Config::Config *>(res)};
            mConfigNeedShown = config;
        }}.detach();
    }, eID_Edit_Config);
    */

    /*
    Bind(wxEVT_BUTTON, [&](wxCommandEvent&) { 
        auto addDialog{AddConfig{this}};
        if (addDialog.ShowModal() != wxID_OK) return;

        mNotifyData.notify(ID_AsyncStart);

        auto importPath{static_cast<filepath>(addDialog.importPath_)};
        auto name{static_cast<string>(addDialog.configName_)};
        std::thread{[this, importPath, name]() {
            Defer defer{[&]() { mNotifyData.notify(ID_AsyncDone); }};

            if (importPath.empty()) {
                auto res{Config::open(name)};
                if (auto *err = std::get_if<string>(&res)) {
                    pcui::showMessage(*err, _("Failed Creating Config"));
                    return;
                }
                auto& config{*std::get<Config::Config *>(res)};
                config.save();
                config.close();
            } else {
                auto err{Config::import(name, importPath)};
                if (err) {
                    pcui::showMessage(*err, _("Cannot Import Config"));
                    return;
                }
            }

            updateConfigChoices();
            configSelection = name;
        }}.detach();
    }, eID_Add_Config);
    */

    /*
    Bind(wxEVT_BUTTON, [&](wxCommandEvent &) {
        if (pcui::showMessage(
                _("Are you sure you want to deleted the selected configuration?") +
                "\n\n" +
                _("This action cannot be undone!"),
                _("Delete Config"),
                wxYES_NO | wxNO_DEFAULT | wxCENTER,
                this) == wxYES
           ) {
            auto *config{Config::getIfOpen(configSelection)};
            for (auto iter{mEditors.begin()}; iter != mEditors.end(); ++iter) {
                auto *editor{*iter};
                if (&editor->getOpenConfig() == config) {
                    editor->Destroy();
                    mEditors.erase(iter);
                    break;
                }
            }
            Config::remove(static_cast<string>(configSelection));

            updateConfigChoices();
        }
    }, eID_Remove_Config);
    */
}

/*
void MainMenu::updateConfigChoices() {
    vector<string> choices{.ToStdString()};
    const auto configList{Config::fetchListFromDisk()};
    choices.insert(choices.end(), configList.begin(), configList.end());
    configSelection.setChoices(std::move(choices));
    if (configSelection == -1) configSelection = 0;
}
*/

/*
void MainMenu::handleNotification(uint32 id) {
    bool rebound{id == pcui::Notifier::eID_Rebound};
    if (rebound or id == ID_ConfigSelection) {
    } 
    if (rebound or id == ID_BoardSelection) {
        FindWindow(ID_ApplyChanges)->Enable(
            configSelection != 0 and boardSelection != 0
        );

        bool canOpenSerial{boardSelection != 0};
#       if defined _WIN32 or defined __linux__
        const auto bootloaderIdx{boardSelection.choices().size() - 1};
        canOpenSerial &= boardSelection != bootloaderIdx;
#       endif
        FindWindow(ID_OpenSerial)->Enable(canOpenSerial);
    }

    if (id == ID_AsyncStart) {
        wxSetCursor(wxCURSOR_WAIT);
    }
    if (rebound or id == ID_AsyncDone) {
        if (mConfigNeedShown != nullptr) {
            EditorWindow *editor{nullptr};

            for (auto *listedEditor : mEditors) {
                if (&listedEditor->getOpenConfig() == mConfigNeedShown) {
                    editor = listedEditor;
                    break;
                }
            }

            if (editor == nullptr) {
                editor = new EditorWindow(this, *mConfigNeedShown);
                mEditors.push_back(editor);
            }

            editor->Show();
            editor->Raise();
            mConfigNeedShown = nullptr;
        }

        wxSetCursor(wxNullCursor);
    }
}
*/

void MainMenu::createMenuBar() {
    auto *file{new wxMenu};
    file->Append(eID_Manage_Versions, _("Manage Versions..."));
    file->Append(eID_Update_Manifest, _("Update Channel..."));
    file->AppendSeparator();
    file->Append(eID_Logs, _("Show Logs..."));
    file->Append(wxID_ABOUT);
    file->Append(eID_Licenses, _("Licensing Information"));
    file->Append(wxID_EXIT);

    auto* menuBar{new wxMenuBar};
    menuBar->Append(file, _("&File"));
    pcui::Frame::appendDefaultMenuItems(menuBar);

    const auto helpStr{_("&Help")};
    const auto helpIdx{menuBar->FindMenu(helpStr)};
    auto *help{
        helpIdx == wxNOT_FOUND
            ? new wxMenu 
            : menuBar->GetMenu(helpIdx)
    };
    help->Append(
        eID_Docs,
        _("Guides...\tCtrl+H"),
        _("Open the ProffieConfig guides in your web browser")
    );
    // TODO: Make this a page that has my contact info and a button to go to
    // the issues page.
    help->Append(
        eID_Issue,
        _("Help/Bug Report..."),
        _("Open GitHub to submit issue")
    );
    help->AppendSeparator();
    help->Append(eID_Run_Setup, _("Re-Run Setup"));
    if (helpIdx == wxNOT_FOUND) menuBar->Append(help, helpStr);

    SetMenuBar(menuBar);
}

pcui::DescriptorPtr MainMenu::ui() {
    return pcui::Stack{
      .base_={
        .border_={.size_=10, .dirs_=wxALL},
      },
      .children_={
        pcui::Spacer{.size_=10}(),
        pcui::Stack{
          .orient_=wxHORIZONTAL,
          .children_={
            pcui::Stack{
              .children_={
                pcui::Label{
                  .label_="ProffieConfig",
#                 if defined(__WXGTK__) or defined(__WXMSW__)
                  .style_=wxFontInfo{20}.Bold(),
#                 elif defined (__WXOSX__)
                  .style_=wxFontInfo{30}.Bold(),
#                 endif
                }(),
                pcui::Label{
                  .label_=_("Created by Ryryog25"),
                }(),
              },
            }(),
            pcui::Spacer{.size_=20}(),
            pcui::StretchSpacer{}(),
            pcui::Image{
              .win_={.maxSize_={64, 64}},
              .src_=pcui::Image::LoadDetails{
                .name_="icon",
              }()
            }()
          },
        }(),
        pcui::Spacer{.size_=20}(),
        pcui::Stack{
          .base_={.expand_=true},
          .orient_=wxHORIZONTAL,
          .children_={
            pcui::Choice{
              .win_={.base_={.proportion_=1}},
              .data_=config_.choice_,
              .unselected_=_("Select Config..."),
            }(),
            pcui::Spacer{.size_=5}(),
            pcui::Button{
              .label_=_("Add"),
              .exactFit_=true,
            }(),
            pcui::Spacer{.size_=5}(),
            pcui::Button{
              .win_={
                .enable_=not data::logic::adapt(
                  config_.choice_, data::logic::HasSelection{{-1}}
                ),
              },
              .label_=_("Remove"),
              .exactFit_=true,
            }(),
          },
        }(),
        pcui::Spacer{.size_=10}(),
        pcui::Button{
          .win_={
            .base_={.expand_=true},
            .enable_=not data::logic::adapt(
              config_.choice_, data::logic::HasSelection{{-1}}
            ),
          },
          .label_=_("Edit Selected Configuration"),
        }(),
        pcui::Spacer{.size_=20}(),
        pcui::Stack{
          .base_={.expand_=true},
          .orient_=wxHORIZONTAL,
          .children_={
            pcui::Button{
              .win_={
                .tooltip_=_("Generate an up-to-date list of connected boards."),
              },
              .label_=_("Refresh Boards"),
            }(),
            pcui::Spacer{.size_=5}(),
            pcui::Choice{
              .win_={
                .base_={.proportion_=1},
                .tooltip_=_("Select the Proffieboard to connect to.\nThese IDs are assigned by the OS, and can vary."),
              },
              .data_=board_,
              .unselected_=_("Select Board..."),
            }(),
          }
        }(),
        pcui::Spacer{.size_=10}(),
        pcui::Button{
          .win_={
            .base_={.expand_=true},
            .enable_={
              not data::logic::adapt(
                config_.choice_, data::logic::HasSelection{{-1}}
              ) and not data::logic::adapt(
                board_, data::logic::HasSelection{{-1}}
              )
            },
            .tooltip_=_("Compile and upload the selected configuration to the selected Proffieboard."),
          },
          .label_=_("Apply Selected Configuration to Board"),
        }(),
        pcui::Spacer{.size_=10}(),
        pcui::Button{
          .win_={
            .base_={.expand_=true},
            .enable_=not data::logic::adapt(
              board_, data::logic::HasSelection{{-1}}
            ),
          },
          .label_=_("Open Serial Monitor"),
        }(),
#       ifdef __WXMSW__
        // There's a sizing issue I need to figure out... for now we give it a
        // chin
        pcui::Spacer{.size_=FromDIP(20)}(),
#       endif
      }
    }();

#   if defined _WIN32 or defined __linux__
    boardEntries.emplace_back(_("BOOTLOADER RECOVERY").ToStdString());
#   endif
}

void MainMenu::removeEditor(EditorWindow *editor) {
    for (auto it{mEditors.begin()}; it != mEditors.end(); ++it) {
        if (*it == editor) {
            mEditors.erase(it);
            break;
        }
    }
}

