//
//  Parameters.swift
//  Revo
//
//  Created by Lucy Nyuu on 2025-10-18.
//

import Foundation
import AudioToolbox

let RevoParameterSpecs = ParameterTreeSpec {
    ParameterGroupSpec(identifier: "global", name: "Global") {
        ParameterSpec(
            address: .timeinterval,
            identifier: "timeinterval",
            name: "Time Interval",
            units: .seconds,
            valueRange: 0.0...3.0,
            defaultValue: 0.5
        )
        ParameterSpec(
            address: .dry,
            identifier: "dry",
            name: "Dry Mix",
            units: .percent,
            valueRange: 0.0...100.0,
            defaultValue: 100.0
        )
        
        ParameterSpec(
            address: .wet,
            identifier: "wet",
            name: "Wet Mix",
            units: .percent,
            valueRange: 0.0...100.0,
            defaultValue: 50.0
        )
        
        ParameterSpec(
            address: .feedback,
            identifier: "feedback",
            name: "Feedback",
            units: .percent,
            valueRange: 0.0...100.0,
            defaultValue: 25.0
        )
        
        ParameterSpec(
            address: .zeno,
            identifier: "zeno",
            name: "Zeno Mode",
            units: .boolean,
            valueRange: 0.0...1.0,
            defaultValue: 0.0
        )
        
        ParameterSpec(
            address: .pingpong,
            identifier: "pingpong",
            name: "Ping Pong",
            units: .boolean,
            valueRange: 0.0...1.0,
            defaultValue: 0.0
        )
        
        ParameterSpec(
            address: .frequency,
            identifier: "frequency",
            name: "Frequency",
            units: .hertz,
            valueRange: 0.0...5.0,
            defaultValue: 0.25
        )
    }
}

extension ParameterSpec {
    init(
        address: RevoParameterAddress,
        identifier: String,
        name: String,
        units: AudioUnitParameterUnit,
        valueRange: ClosedRange<AUValue>,
        defaultValue: AUValue,
        unitName: String? = nil,
        flags: AudioUnitParameterOptions = [AudioUnitParameterOptions.flag_IsWritable, AudioUnitParameterOptions.flag_IsReadable],
        valueStrings: [String]? = nil,
        dependentParameters: [NSNumber]? = nil
    ) {
        self.init(address: address.rawValue,
                  identifier: identifier,
                  name: name,
                  units: units,
                  valueRange: valueRange,
                  defaultValue: defaultValue,
                  unitName: unitName,
                  flags: flags,
                  valueStrings: valueStrings,
                  dependentParameters: dependentParameters)
    }
}
