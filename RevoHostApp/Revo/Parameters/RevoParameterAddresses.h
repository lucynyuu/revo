//
//  RevoParameterAddresses.h
//  Revo
//
//  Created by Lucy Nyuu on 2025-10-18.
//

#pragma once

#include <AudioToolbox/AUParameters.h>

typedef NS_ENUM(AUParameterAddress, RevoParameterAddress) {
    timeinterval = 0,
    dry = 1,
    wet = 2,
    feedback = 3,
    zeno = 4,
    pingpong = 5,
    frequency = 6
};
