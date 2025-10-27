//
//  ParameterCheckBox.swift
//  Revo
//
//  Created by Lucy Nyuu on 2025-10-21.
//

import SwiftUI

/// A SwiftUI Slider container which is bound to an ObservableAUParameter
///
/// This view wraps a SwiftUI Slider, and provides it relevant data from the Parameter, like the minimum and maximum values.
struct ParameterCheckBox: View {
    @State var param: ObservableAUParameter
    @Binding var isChecked: Bool
        
    var body: some View {
        Toggle(isOn: Binding(
            get: { param.value > 0.5 },
            set: { newValue in
                param.value = newValue ? 1.0 : 0.0
                param.onEditingChanged(true)
                param.onEditingChanged(false)
                isChecked = newValue
            }
        )) {
            Text(param.displayName)
                .foregroundColor(.white)
        }
        //.toggleStyle(GlowingCheckboxStyle(glowColor: .white, glowRadius: 8))
        .toggleStyle(.checkbox)
        .padding(.horizontal, 10)
        .padding(.vertical, 2)
    }
}

struct GlowingCheckboxStyle: ToggleStyle {
    var glowColor: Color = .white
    var glowRadius: CGFloat = 8
    var size: CGFloat = 18

    func makeBody(configuration: Configuration) -> some View {
        HStack {
            ZStack {
                RoundedRectangle(cornerRadius: 4)
                    .stroke(glowColor, lineWidth: 2)
                    .frame(width: size, height: size)
                    .shadow(color: configuration.isOn ? glowColor.opacity(0.8) : .clear,
                            radius: configuration.isOn ? glowRadius : 0)
                    .animation(.easeOut(duration: 0.15), value: configuration.isOn)
                    .onTapGesture {
                        configuration.isOn.toggle()
                    }

                if configuration.isOn {
                    RoundedRectangle(cornerRadius: 4)
                        .fill(glowColor.opacity(0.25))
                        .frame(width: size, height: size)
                        .blur(radius: 0.5)
                }
            }

            configuration.label
                .foregroundColor(.white)
        }
        .padding(.vertical, 2)
    }
}
