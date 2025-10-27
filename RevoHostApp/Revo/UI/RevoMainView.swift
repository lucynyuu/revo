//
//  RevoMainView.swift
//  Revo
//
//  Created by Lucy Nyuu on 2025-10-18.
//

import SwiftUI

struct RevoMainView: View {
    var parameterTree: ObservableAUParameterGroup
    @State private var isZenoEnabled = false
    @State private var isPingPongEnabled = false
    
    var body: some View {
        VStack {
            VStack(spacing: 2) {
                HStack(spacing: 0) {
                    ZStack {
                        Text("Re")
                            .font(.system(size: 32, weight: .bold))
                            .foregroundStyle(.blue)
                            .blur(radius: 10)
                            .shadow(color: .blue, radius: 4)

                        Text("Re")
                            .font(.system(size: 32, weight: .bold))
                            .foregroundStyle(.blue)
                    }
                    Text("vo")
                        .foregroundColor(.white)
                }
                .font(.system(size: 32, weight: .bold))
                Divider()
                    .frame(height: 2)
                    .background(Color.white.opacity(1))
            }
            .offset(y: -10)
            VStack(spacing: 12) {
                ParameterSlider(param: parameterTree.global.timeinterval)
                ParameterSlider(param: parameterTree.global.feedback)
                    .disabled(isZenoEnabled)
                HStack {
                    ParameterCheckBox(param: parameterTree.global.zeno, isChecked: $isZenoEnabled)
                    ParameterCheckBox(param: parameterTree.global.pingpong, isChecked: $isPingPongEnabled)
                    ParameterSlider(param: parameterTree.global.frequency)
                        .disabled(!isPingPongEnabled)
                }
                ParameterSlider(param: parameterTree.global.dry)
                ParameterSlider(param: parameterTree.global.wet)
            }
            .padding()
            .background(
                RoundedRectangle(cornerRadius: 10)
                    .stroke(Color.white, lineWidth: 2)
            )
            .padding(.horizontal, 5)
        }
        .padding()
        .frame(maxWidth: .infinity, maxHeight: .infinity, alignment: .top)
        .background(Color(red: 0.16, green: 0.16, blue: 0.16))
        .ignoresSafeArea()
    }
}
