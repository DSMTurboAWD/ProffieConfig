#include "presetarray.hpp"
/*
 * ProffieConfig, All-In-One Proffieboard Management Utility
 * Copyright (C) 2023-2026 Ryan Ogurek
 *
 * proffieconfig/editor/dialogs/presetarray.cpp
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

class RenameArrayDlg : pcui::Dialog {
public:
    RenameArrayDlg(
        wxWindow *parent,
        Config::Config& config,
        Config::PresetArray& array,
        const wxString& title,
        bool buttons = false
    ) : wxDialog(parent, wxID_ANY, title),
        NotifyReceiver(this, array.notifyData),
        mConfig{config}, mArray{array} {
        auto *sizer{new wxBoxSizer(wxVERTICAL)};
        auto *entry{new pcui::Text(
            this,
            array.name,
            0,
            false,
            _("Name")
        )};
        auto *emptyText{new wxStaticText(
            this,
            eID_Empty_Text,
            _("Empty Array Name")
        )};
        auto *dupText{new wxStaticText(
            this,
            eID_Dup_Text,
            _("Duplicate Array Name")
        )};
        sizer->Add(
            entry,
            wxSizerFlags().Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10)
        );
        sizer->Add(
            emptyText,
            wxSizerFlags().Right().Border(wxALL, 5)
                .DoubleBorder(wxRIGHT)
        );
        sizer->Add(
            dupText,
            wxSizerFlags().Right().Border(wxALL, 5)
                .DoubleBorder(wxRIGHT)
        );
        if (buttons) {
            sizer->Add(
                CreateStdDialogButtonSizer(wxOK | wxCANCEL),
                wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM, 10)
            );
        } else {
            sizer->AddSpacer(10);
        }

        entry->SetFocus();

        Bind(wxEVT_CHAR_HOOK, [this](wxKeyEvent& evt) {
            if (evt.GetKeyCode() == WXK_ESCAPE) EndModal(wxID_CANCEL);

            auto *okButton{FindWindow(wxID_OK)};
            if (
                    (not okButton or okButton->IsEnabled()) and 
                    evt.GetKeyCode() == WXK_RETURN and
                    evt.GetKeyCode() == WXK_NUMPAD_ENTER
               ) {
                EndModal(wxID_OK);
            }

            evt.Skip();
        });

        SetSizer(sizer);
        initializeNotifier();
    }

private:
    Config::Config& mConfig;
    Config::PresetArray& mArray;

    enum {
        eID_Empty_Text = 2,
        eID_Dup_Text,
    };

    void handleNotification(uint32) final {
        bool duplicate{false};
        for (const auto& otherArray : mConfig.presetArrays.arrays()) {
            if (&*otherArray == &mArray) continue;

            const auto otherArrayName{static_cast<string>(otherArray->name)};
            const auto arrayName{static_cast<string>(mArray.name)};
            if (arrayName == otherArrayName) {
                duplicate = true;
                break;
            }
        }
        bool empty{static_cast<string>(mArray.name).empty()};

        FindWindow(eID_Empty_Text)->Show(empty);
        FindWindow(eID_Dup_Text)->Show(duplicate);
        auto *okButton{FindWindow(wxID_OK)};
        if (okButton) okButton->Enable(not duplicate and not empty);

        SetMinSize({300, -1});
        Layout();
        Fit();
    }
};

