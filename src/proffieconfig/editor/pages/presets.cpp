#include "presets.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2023-2026 Ryan Ogurek
 *
 * proffieconfig/editor/pages/presets.cpp
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

#include <wx/gdicmn.h>
#include <wx/msgdlg.h>

#include "ui/controls/button.hpp"
#include "ui/controls/choice.hpp"
#include "ui/layout/group.hpp"
#include "ui/layout/spacer.hpp"
#include "ui/layout/stack.hpp"
#include "ui/static/image.hpp"
#include "ui/types.hpp"

PresetsPage::PresetsPage(config::Config& config) : mConfig{config} {

}

pcui::DescriptorPtr PresetsPage::ui() {
    return pcui::Stack{
      .orient_=wxHORIZONTAL,
      .children_={
        selection(),
      }
    }();

    /*
    auto *sizer{new wxBoxSizer(wxHORIZONTAL)};
    auto& config{mParent->getOpenConfig()};

    auto *presetConfigSizer{new wxBoxSizer(wxVERTICAL)};
    presetConfigSizer->SetMinSize(wxSize(200, -1));

    auto *nameInput{new pcui::Text(
        this,
        config.presetArrays.nameProxy,
        wxTE_PROCESS_ENTER,
        true,
        _("Preset Name")
    )};
    nameInput->SetToolTip(_(
        "The name for the preset.\n"
        "This appears on the OLED screen if no bitmap is supplied, otherwise it's just for reference.\n"
        "\"\\n\" means \"enter.\" Hitting \"enter\" will insert \"\\n\" which means a new line in the text displayed on the OLED.\n"
        "For example, \"my\\npreset\" will be displayed on the OLED as two lines, the first being \"my\" and the second being \"preset.\""
    ));

    auto *dirInput{new pcui::Text(
        this,
        config.presetArrays.dirProxy,
        0,
        false,
        _("Font Directory")
    )};
    dirInput->SetToolTip(_(
        "The path of the folder on the SD card where the font is stored.\n"
        "If the font folder is inside another folder, it must be indicated by something like \"folderName/fontFolderName\".\n"
        "In order to specify multiple directories (for example, to include a \"common\" directory), use a semicolon (;) to separate the folders (e.g. \"fontFolderName;common\")."
    ));

    auto *trackSizer{new wxBoxSizer(wxHORIZONTAL)};
    auto *trackInput{new pcui::Text(
        this,
        config.presetArrays.trackProxy,
        0,
        false,
        _("Track File")
    )};
    trackInput->SetToolTip(_(
        "The path of the track file on the SD card. May be empty.\n"
        "If the track is directly inside one of the folders specified in \"Font Directory\" then only the name of the track file is required."
    ));

    auto *wavText{new wxStaticText(
        this,
        eID_Wav_Text,
        ".wav"
    )};
    wavText->Hide();
    trackSizer->Add(trackInput, wxSizerFlags(1));
    trackSizer->Add(wavText, wxSizerFlags().Bottom());

    mInjectionsSizer = new pcui::StaticBox(wxVERTICAL, this, _("Injections"));

    presetConfigSizer->Add(nameInput, wxSizerFlags().Expand());
    presetConfigSizer->AddSpacer(5);
    presetConfigSizer->Add(dirInput, wxSizerFlags().Expand());
    presetConfigSizer->AddSpacer(5);
    presetConfigSizer->Add(trackSizer, wxSizerFlags().Expand());
    presetConfigSizer->AddSpacer(20);
    presetConfigSizer->Add(mInjectionsSizer, wxSizerFlags(1).Expand());

    auto *stylesSizer{new wxBoxSizer(wxVERTICAL)};
    auto *styleDisplay{new pcui::Choice(
        this,
        config.presetArrays.styleDisplay,
        _("Display")
    )};
    styleDisplay->SetToolTip(_("Show blade listing corresponding to the selected blade array."));

    auto *styleList{new pcui::List(
        this,
        config.presetArrays.styleSelectProxy,
        _("Blades")
    )};
    stylesSizer->Add(styleDisplay, wxSizerFlags().Expand());
    stylesSizer->AddSpacer(5);
    stylesSizer->Add(styleList, wxSizerFlags(1).Expand());

    auto *styleCommentSplit{new wxSplitterWindow(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxSP_3DSASH | wxSP_LIVE_UPDATE
    )};

    auto *commentInput{new pcui::Text(
        styleCommentSplit,
        config.presetArrays.commentProxy,
        wxTE_MULTILINE | wxNO_BORDER,
        false,
        _("Comments")
    )};
    commentInput->SetToolTip(_(
        "Any comments about the blade style goes here.\n"
        "This doesn't affect the blade style at all, but can be a place for helpful notes!"
    ));

    auto *styleInput{new pcui::Text(
        styleCommentSplit,
        config.presetArrays.styleProxy,
        wxTE_DONTWRAP | wxTE_MULTILINE | wxNO_BORDER,
        false,
        _("Blade Style")
    )};
    styleInput->styleMonospace();
    styleInput->SetToolTip(_(
        "Your blade style goes here.\n"
        "This is the code which sets up what animations and effects your blade (or other LED) will do.\n"
        "For getting/creating blade styles, see the Documentation (in \"Help->Documentation...\")."
    ));

    styleCommentSplit->SetMinSize(wxSize{500, -1});
    styleCommentSplit->SetMinimumPaneSize(60);
    styleCommentSplit->SplitHorizontally(commentInput, styleInput);

    sizer->Add(presetSelectionSizer, wxSizerFlags().Expand());
    sizer->AddSpacer(10);
    sizer->Add(presetConfigSizer, wxSizerFlags().Expand());
    sizer->AddSpacer(10);
    sizer->Add(stylesSizer, wxSizerFlags().Expand());
    sizer->AddSpacer(10);
    sizer->Add(styleCommentSplit, wxSizerFlags(1).Expand());

    SetSizerAndFit(sizer);
    */
}

pcui::DescriptorPtr PresetsPage::selection() {
#   ifdef __WXOSX__
    wxSize arrangeButtonSize{20, 25};
#   else
    wxSize arrangeButtonSize{15, 25};
#   endif

    return pcui::Stack{
      .orient_=wxVERTICAL,
      .children_={
        pcui::Group{
          .base_={.expand_=true},
          .label_=_("Presets Array"),
          .orient_=wxVERTICAL,
          .children_={
            pcui::Stack{
              .base_={.expand_=true},
              .orient_=wxHORIZONTAL,
              .children_={
                pcui::Choice{
                  .win_={.base_={.proportion_=1}},
                  .data_=mArraySel.choice_,
                }(),
                pcui::Button{
                  .label_="ISS",
                }(),
                pcui::Spacer{.size_=8}(),
                pcui::Button{
                  .bitmap_=pcui::Image::LoadDetails{
                    .name_="edit",
                    .size_={.dim_=16},
                    .color_=wxSYS_COLOUR_WINDOWTEXT,
                  }(),
                }(),
              }
            }(),
            pcui::Spacer{.size_=8}(),
            pcui::Stack{
              .base_={.expand_=true},
              .orient_=wxHORIZONTAL,
              .children_={
                pcui::Button{
                  .label_=_("Add"),
                }(),
                pcui::Spacer{.size_=8}(),
                pcui::Button{
                  .label_=_("Add"),
                }(),
              }
            }(),
          }
        }(),
        pcui::Spacer{.size_=10}(),
        pcui::Group{
          .base_={.expand_=true, .proportion_=1},
          .label_=_("Presets"),
          .orient_=wxHORIZONTAL,
          .children_={
            pcui::Stack{
              .orient_=wxVERTICAL,
              .children_={
                pcui::Choice{
                  .win_={
                    .base_={
                      .minSize_={-1, 300},
                      .proportion_=1,
                    },
                    .tooltip_=_("The currently-selected preset array to be edited.\nEach preset array has unique presets."),
                  },
                  .data_=mPresetSel.choice_,
                  .style_=pcui::Choice::List{},
                }(),
                pcui::Stack{
                  .orient_=wxHORIZONTAL,
                  .children_={
                    pcui::Button{
                      .win_={.base_={.minSize_=arrangeButtonSize}},
                      .label_="+",
                      .style_=pcui::Button::Style::Companion,
                      .exactFit_=true,
                    }(),
                    pcui::Button{
                      .win_={.base_={.minSize_=arrangeButtonSize}},
                      .label_="-",
                      .style_=pcui::Button::Style::Companion,
                      .exactFit_=true,
                    }(),
                  }
                }()
              }
            }(),
            pcui::Stack{
              .orient_=wxVERTICAL,
              .children_={
                pcui::Button{
                  .win_={.base_={.minSize_=arrangeButtonSize}},
                  .label_=L"\u2191", // Up Arrow
                  .style_=pcui::Button::Style::Companion,
                  .exactFit_=true,
                }(),
                pcui::Button{
                  .win_={.base_={.minSize_=arrangeButtonSize}},
                  .label_=L"\u2193", // Down Arrow
                  .style_=pcui::Button::Style::Companion,
                  .exactFit_=true,
                }(),
                pcui::Button{
                  .win_={.base_={.minSize_=arrangeButtonSize}},
                  .label_=L"\u29C9", // ⧉ Double Squares
                  .style_=pcui::Button::Style::Companion,
                  .exactFit_=true,
                }(),
              }
            }(),
          }
        }(),
      }
    }();
}

/*
void PresetsPage::bindEvents() {
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto& config{mParent->getOpenConfig()};
        auto& presetArray{
            config.presetArrays.array(config.presetArrays.selection)
        };
        presetArray.addPreset();
        const auto lastIdx{
            static_cast<int32>(presetArray.presets().size() - 1)
        };
        presetArray.selection = lastIdx;
    }, eID_Add_Preset);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto& presetArrays{mParent->getOpenConfig().presetArrays};
        if (presetArrays.selection == -1) return;
        auto& presetArray{presetArrays.array(presetArrays.selection)};
        if (presetArray.selection == -1) return;

        presetArray.removePreset(presetArray.selection);
    }, eID_Remove_Preset);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto& presetArrays{mParent->getOpenConfig().presetArrays};
        if (presetArrays.selection == -1) return;
        auto& presetArray{presetArrays.array(presetArrays.selection)};
        if (presetArray.selection == -1) return;

        presetArray.movePresetUp(presetArray.selection);
    }, eID_Move_Preset_Up);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto& presetArrays{mParent->getOpenConfig().presetArrays};
        if (presetArrays.selection == -1) return;
        auto& presetArray{presetArrays.array(presetArrays.selection)};
        if (presetArray.selection == -1) return;

        presetArray.movePresetDown(presetArray.selection);
    }, eID_Move_Preset_Down);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto& presetArrays{mParent->getOpenConfig().presetArrays};
        if (presetArrays.selection == -1) return;
        auto& presetArray{presetArrays.array(presetArrays.selection)};
        if (presetArray.selection == -1) return;

        presetArray.duplicatePreset(presetArray.selection);
    }, eID_Duplicate_Preset);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto& config{mParent->getOpenConfig()};
        auto& presetArray{config.presetArrays.array(
            config.presetArrays.selection
        )};

        RenameArrayDlg renameDlg(
            mParent,
            config,
            presetArray,
            _("Rename Preset Array")
        );
        renameDlg.ShowModal();
    }, eID_Rename_Array);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto& config{mParent->getOpenConfig()};

        Config::PresetArray array(config);
        RenameArrayDlg dlg(
            mParent,
            config,
            array,
            _("Add Preset Array"),
            true
        );
        if (wxID_OK == dlg.ShowModal()) {
            config.presetArrays.addArray(array.name);
            const auto lastIdx{static_cast<int32>(
                config.presetArrays.arrays().size() - 1
            )};
            config.presetArrays.selection = lastIdx;
        }

    }, eID_Add_Array);
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        auto& presetArrays{mParent->getOpenConfig().presetArrays};
        presetArrays.removeArray(presetArrays.selection);
    }, eID_Remove_Array);
}

void PresetsPage::handleNotification(uint32 id) {
    bool rebound{id == pcui::Notifier::eID_Rebound};

    auto& presetArrays{mParent->getOpenConfig().presetArrays};
    if (rebound or id == Config::PresetArrays::NOTIFY_INJECTIONS) {
        rebuildInjections();
    }
    if (rebound or id == Config::PresetArrays::NOTIFY_SELECTION) {
        bool hasSelection{presetArrays.selection != -1};
        FindWindow(eID_Remove_Array)->Enable(hasSelection);
        FindWindow(eID_Rename_Array)->Enable(hasSelection);
        FindWindow(eID_Add_Preset)->Enable(hasSelection);
    }
    if (
            rebound or
            id == Config::PresetArrays::NOTIFY_ARRAY_NAME or
            id == Config::PresetArrays::NOTIFY_SELECTION
       ) {
        auto *issueButton{FindWindow(eID_Issue_Button)};
        // This is late for sizing reasons
        issueButton->SetLabel(L"\u26D4");

        if (presetArrays.selection == -1) {
            issueButton->Hide();
        } else {
            auto& selectedArray{presetArrays.array(presetArrays.selection)};
            auto selectedArrayName{static_cast<string>(selectedArray.name)};
            bool duplicate{false};
            for (const auto& array : presetArrays.arrays()) {
                if (&*array == &selectedArray) continue;

                if (static_cast<string>(array->name) == selectedArrayName) {
                    duplicate = true;
                    break;
                }
            }
            issueButton->Show(
                static_cast<string>(selectedArray.name).empty() or duplicate
            );
        }
    }
    if (
            rebound or
            id == Config::PresetArrays::NOTIFY_PRESETS or
            id == Config::PresetArrays::NOTIFY_SELECTION
       ) {
        if (presetArrays.selection == -1) {
            FindWindow(eID_Move_Preset_Up)->Disable();
            FindWindow(eID_Move_Preset_Down)->Disable();
            FindWindow(eID_Duplicate_Preset)->Disable();
            FindWindow(eID_Remove_Preset)->Disable();
        } else {
            auto& presetArray{presetArrays.array(presetArrays.selection)};
            bool hasPresetSelection{presetArray.selection != -1};

            bool notFirst{presetArray.selection > 0};
            const auto lastIdx{presetArray.selection.choices().size() - 1};
            bool notLast{presetArray.selection < lastIdx};

            auto *movePresetUp{FindWindow(eID_Move_Preset_Up)};
            movePresetUp->Enable(hasPresetSelection and notFirst);
            auto *movePresetDown{FindWindow(eID_Move_Preset_Down)};
            movePresetDown->Enable(hasPresetSelection and notLast);
            auto *removePreset{FindWindow(eID_Remove_Preset)};
            removePreset->Enable(hasPresetSelection);
            auto *dupPreset{FindWindow(eID_Duplicate_Preset)};
            dupPreset->Enable(hasPresetSelection);
        }
    }
    if (
            rebound or
            id == Config::PresetArrays::NOTIFY_TRACK_INPUT or
            id == Config::PresetArrays::NOTIFY_PRESETS or
            id == Config::PresetArrays::NOTIFY_SELECTION
       ) {
        auto hasInput{
            presetArrays.trackProxy.data() and
            not static_cast<string>(*presetArrays.trackProxy.data()).empty()
        };
        FindWindow(eID_Wav_Text)->Show(hasInput);
    }

    Layout();
}

void PresetsPage::rebuildInjections() {
    mInjectionsSizer->Clear(true);

    auto& config{mParent->getOpenConfig()};

    for (const auto& injection : config.presetArrays.injections()) {
        auto *injectionSizer{new wxBoxSizer(wxHORIZONTAL)};
        auto *injectionText{new wxStaticText(
            mInjectionsSizer->childParent(),
            wxID_ANY,
            injection->filename
        )};
        auto *editButton{new wxButton(
            mInjectionsSizer->childParent(),
            wxID_ANY,
            _("Edit"),
            wxDefaultPosition,
            wxDefaultSize,
            wxBU_EXACTFIT
        )};
        auto *deleteButton{new wxButton(
            mInjectionsSizer->childParent(),
            wxID_ANY,
            _("Delete"),
            wxDefaultPosition,
            wxDefaultSize,
            wxBU_EXACTFIT
        )};

        editButton->Bind(wxEVT_BUTTON, [&injection](wxCommandEvent&) {
            wxLaunchDefaultApplication(
                (paths::injectionDir() / injection->filename).native()
            );
        });

        deleteButton->Bind(wxEVT_BUTTON, [this, &injection](wxCommandEvent&) {
            auto res{pcui::showMessage(
                _("This action cannot be undone!"),
                _("Delete Injection"),
                wxYES_NO | wxNO_DEFAULT
            )};
            if (wxNO == res) return;

            mParent->getOpenConfig().presetArrays.removeInjection(*injection);
        });

        injectionSizer->Add(injectionText, wxSizerFlags(1).Center());
        injectionSizer->AddSpacer(20);
        injectionSizer->Add(editButton);
        injectionSizer->AddSpacer(10);
        injectionSizer->Add(deleteButton);

        mInjectionsSizer->Add(injectionSizer, wxSizerFlags().Expand());
    }

    mInjectionsSizer->Show(not mInjectionsSizer->IsEmpty());
    mInjectionsSizer->Layout();
    Layout();
}
*/

