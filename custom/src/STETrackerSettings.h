/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "SettingsGroup.h"

class STETrackerSettings : public SettingsGroup
{
    Q_OBJECT
    
public:
    STETrackerSettings(QObject* parent = nullptr);

    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(frequency)
    DEFINE_SETTINGFACT(gain)
    DEFINE_SETTINGFACT(autoGain)
    DEFINE_SETTINGFACT(minValidPulse)
    DEFINE_SETTINGFACT(filterMinPulse)
};
