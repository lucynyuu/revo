//
//  ParameterSlider.swift
//  Revo
//
//  Created by Lucy Nyuu on 2025-10-18.
//

import SwiftUI

/// A SwiftUI Slider container which is bound to an ObservableAUParameter
///
/// This view wraps a SwiftUI Slider, and provides it relevant data from the Parameter, like the minimum and maximum values.
struct ParameterSlider: View {
    @State var param: ObservableAUParameter
    
    var specifier: String {
        switch param.unit {
            case .seconds:
                return "%.2fs"
            case .hertz:
                return "%.3fhz"
            default:
                return "%.0f%%"
        }
    }
    
    var body: some View {
        VStack (alignment: .leading) {
            Text("\(param.displayName): \(param.value, specifier: specifier)")
                .foregroundColor(.white)
                .multilineTextAlignment(.leading)
            Slider(
                value: $param.value,
                in: param.min...param.max,
                onEditingChanged: param.onEditingChanged,
                minimumValueLabel: Text("\(param.min, specifier: specifier)").foregroundColor(.white),
                maximumValueLabel: Text("\(param.max, specifier: specifier)").foregroundColor(.white)
            ) {
                EmptyView()
            }
            .tint(.blue)
            .accessibility(identifier: param.displayName)
        }
        .padding(.horizontal, 10)
        .padding(.vertical, 2)
    }
}
