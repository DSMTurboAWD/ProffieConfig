#include "addconfig.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2025-2026 Ryan Ogurek
 *
 * proffieconfig/mainmenu/dialogs/addconfig.cpp
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

#include <wx/dialog.h>
#include <wx/sysopt.h>
#include <wx/filedlg.h>

#include "config/config.hpp"
#include "data/helpers/exclusive.hpp"
#include "data/logic/adapter.hpp"
#include "data/logic/operators.hpp"
#include "ui/build.hpp"
#include "ui/controls/button.hpp"
#include "ui/controls/filepicker.hpp"
#include "ui/controls/segmented.hpp"
#include "ui/controls/text.hpp"
#include "ui/helpers/dialog_buttons.hpp"
#include "ui/layout/spacer.hpp"
#include "ui/layout/stack.hpp"
#include "ui/static/label.hpp"
#include "ui/static/image.hpp"
#include "ui/types.hpp"
#include "utils/parent.hpp"

AddConfig::AddConfig(MainMenu *parent) : 
    Dialog(
        parent,
        wxID_ANY,
        _("Add New Config")
    ) {
#   ifdef __WXOSX__
    // TODO: This should really go elsewhere... but where?
    wxSystemOptions::SetOption(wxOSX_FILEDIALOG_ALWAYS_SHOW_TYPES, true);
#   endif

    FindWindow(wxID_OK)->Disable();

    pcui::build(this, ui());
    bindEvents();
}

AddConfig::~AddConfig() {
    pcui::teardown(this);
}

void AddConfig::bindEvents() {
    configName_.responder().onChange_ = [](
        const data::String::ROContext& ctxt
    ) {
        auto& self{utils::parent<&AddConfig::configName_>(
            const_cast<data::String&>(ctxt.model<data::String>())
        )};

        bool dupName{false};
        data::Vector::ROContext list{config::list()};
        for (const auto& model : list.children()) {
            auto& exist{static_cast<config::Info&>(*model)};
            data::String::ROContext existName{exist.name()};
            if (existName.val() != ctxt.val()) continue;

            dupName = true;
            break;
        }
        data::Bool::Context{self.mDupName}.set(dupName);

        bool nameEmpty{ctxt.val().empty()};
        bool nameBadChars{
            ctxt.val().find_first_of(".\\,/!#$%^&*|?<>\"'") != std::string::npos
        };

        data::Bool::Context{self.mNameValid}.set(
            not nameEmpty and
            not dupName and
            not nameBadChars
        );
    };

    importPath_.responder().onChange_ = [](
        const data::String::ROContext& ctxt
    ) {
        auto& self{utils::parent<&AddConfig::configName_>(
            const_cast<data::String&>(ctxt.model<data::String>())
        )};

        data::Bool::Context{self.mNeedImportPath}.set(ctxt.val().empty());
    };
}

pcui::DescriptorPtr AddConfig::ui() {
    return pcui::Stack{
      .base_={.minSize_={400, -1}},
      .children_={
        pcui::Segmented{
          .data_=mode_,
          .labels_={
            pcui::Segmented::Label{
              .text_=_("Create New Config"),
              .image_=pcui::Image::LoadDetails{
                .name_="new",
                .size_={.dim_=32},
              }(),
            },
            pcui::Segmented::Label{
              .text_=_("Import Existing Config"),
              .image_=pcui::Image::LoadDetails{
                .name_="import",
                .size_={.dim_=32},
              }(),
            },
          },
        }(),
        pcui::Label{
          .win_={.show_=data::logic::adapt(mode_[eMode_Import])},
          .label_=_("Configuration to Import"),
        }(),
        pcui::FilePicker{
          .win_={
            .base_{.expand_=true},
            .show_=data::logic::adapt(mode_[eMode_Import])
          },
          .data_=importPath_,
          .message_=_("Choose Configuration File to Import"),
          .wildcard_=_("ProffieOS Configuration") + " (*.h)|*.h",
          .mode_=pcui::FilePicker::Open{},
        }(),
        pcui::Spacer{.size_=10}(),
        pcui::Label{
          .label_=_("Configuration Name"),
        }(),
        pcui::Text{
          .win_={.base_{.expand_=true}},
          .data_=configName_,
        }(),
        pcui::Label{
          .win_={
            .base_={
              .border_={.size_=10, .dirs_=wxRIGHT},
              .align_=wxALIGN_RIGHT,
            },
            .show_=data::logic::adapt(mNameValid)
          },
          .label_=_("Please enter a valid name"),
        }(),
        pcui::Label{
          .win_={
            .base_={
              .border_={.size_=10, .dirs_=wxRIGHT},
              .align_=wxALIGN_RIGHT,
            },
            .show_=data::logic::adapt(mDupName)
          },
          .label_=_("Configuration with same name already exists"),
        }(),
        pcui::Label{
          .win_={
            .base_={
              .border_={.size_=10, .dirs_=wxRIGHT},
              .align_=wxALIGN_RIGHT,
            },
            .show_=data::logic::adapt(mNeedImportPath)
          },
          .label_=_("Please choose a configuration file to import"),
        }(),
        pcui::StretchSpacer{}(),
        pcui::DialogButtons{
          .base_={
            .expand_=true,
            .border_={.size_=10, .dirs_=wxALL},
          },
          .ok_=pcui::Button{
            .win_{
              .enable_={
                data::logic::adapt(mNameValid) and
                (not data::logic::adapt(mode_[eMode_Import]) or
                 not data::logic::adapt(mNeedImportPath))
              },
            },
            .label_=_("Ok"),
          }(),
          .cancel_=pcui::Button{
            .label_=_("Cancel"),
          }(),
        }(),
      },
    }();
}

