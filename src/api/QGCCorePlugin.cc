/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "QGCOptions.h"
#include "QmlComponentInfo.h"
#include "FactMetaData.h"
#include "SettingsManager.h"
#include "AppMessages.h"
#include "QmlObjectListModel.h"
#include "VideoReceiver.h"
#include "QGCLoggingCategory.h"

#if !defined(__mobile__)
#include "QGCQmlWidgetHolder.h"
#endif

#include <QtQml>
#include <QQmlEngine>

/// @file
///     @brief Core Plugin Interface for QGroundControl - Default Implementation
///     @author Gus Grubba <mavlink@grubba.com>

class QGCCorePlugin_p
{
public:
    QGCCorePlugin_p()
    {
    }

    ~QGCCorePlugin_p()
    {
        if(pGeneral)
            delete pGeneral;
        if(pCommLinks)
            delete pCommLinks;
        if(pOfflineMaps)
            delete pOfflineMaps;
#if defined(QGC_GST_TAISYNC_ENABLED)
        if(pTaisync)
            delete pTaisync;
#endif
#if defined(QGC_AIRMAP_ENABLED)
        if(pAirmap)
            delete pAirmap;
#endif
        if(pMAVLink)
            delete pMAVLink;
        if(pConsole)
            delete pConsole;
#if defined(QT_DEBUG)
        if(pMockLink)
            delete pMockLink;
        if(pDebug)
            delete pDebug;
#endif
        if(defaultOptions)
            delete defaultOptions;
    }

    QmlComponentInfo* pGeneral                  = nullptr;
    QmlComponentInfo* pCommLinks                = nullptr;
    QmlComponentInfo* pOfflineMaps              = nullptr;
#if defined(QGC_GST_TAISYNC_ENABLED)
    QmlComponentInfo* pTaisync                  = nullptr;
#endif
#if defined(QGC_AIRMAP_ENABLED)
    QmlComponentInfo* pAirmap                   = nullptr;
#endif
    QmlComponentInfo* pMAVLink                  = nullptr;
    QmlComponentInfo* pConsole                  = nullptr;
    QmlComponentInfo* pHelp                     = nullptr;
#if defined(QT_DEBUG)
    QmlComponentInfo* pMockLink                 = nullptr;
    QmlComponentInfo* pDebug                    = nullptr;
#endif

    QmlComponentInfo*   valuesPageWidgetInfo    = nullptr;
    QmlComponentInfo*   cameraPageWidgetInfo    = nullptr;
    QmlComponentInfo*   videoPageWidgetInfo     = nullptr;
    QmlComponentInfo*   healthPageWidgetInfo    = nullptr;
    QmlComponentInfo*   vibrationPageWidgetInfo = nullptr;

    QGCOptions*         defaultOptions          = nullptr;
    QVariantList        settingsList;
    QVariantList        instrumentPageWidgetList;

    QmlObjectListModel _emptyCustomMapItems;
};

QGCCorePlugin::~QGCCorePlugin()
{
    if(_p) {
        delete _p;
    }
}

QGCCorePlugin::QGCCorePlugin(QGCApplication *app, QGCToolbox* toolbox)
    : QGCTool(app, toolbox)
    , _showTouchAreas(false)
    , _showAdvancedUI(true)
{
    _p = new QGCCorePlugin_p;
}

void QGCCorePlugin::setToolbox(QGCToolbox *toolbox)
{
    QGCTool::setToolbox(toolbox);
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    qmlRegisterUncreatableType<QGCCorePlugin>("QGroundControl.QGCCorePlugin", 1, 0, "QGCCorePlugin", "Reference only");
    qmlRegisterUncreatableType<QGCOptions>("QGroundControl.QGCOptions",       1, 0, "QGCOptions",    "Reference only");
}

QVariantList &QGCCorePlugin::settingsPages()
{
    if(!_p->pGeneral) {
        _p->pGeneral = new QmlComponentInfo(tr("General"),
            QUrl::fromUserInput("qrc:/qml/GeneralSettings.qml"),
            QUrl::fromUserInput("qrc:/res/gear-white.svg"));
        _p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pGeneral)));
        _p->pOfflineMaps = new QmlComponentInfo(tr("Offline Maps"),
            QUrl::fromUserInput("qrc:/qml/OfflineMap.qml"),
            QUrl::fromUserInput("qrc:/res/waves.svg"));
        _p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pOfflineMaps)));
        _p->pConsole = new QmlComponentInfo(tr("Console"),
            QUrl::fromUserInput("qrc:/qml/QGroundControl/Controls/AppMessages.qml"));
        _p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pConsole)));
    }
    return _p->settingsList;
}

QVariantList& QGCCorePlugin::instrumentPages(void)
{
    if (!_p->valuesPageWidgetInfo) {
        _p->valuesPageWidgetInfo    = new QmlComponentInfo(tr("Values"),    QUrl::fromUserInput("qrc:/qml/ValuePageWidget.qml"));
        _p->cameraPageWidgetInfo    = new QmlComponentInfo(tr("Camera"),    QUrl::fromUserInput("qrc:/qml/CameraPageWidget.qml"));
#if defined(QGC_GST_STREAMING)
        _p->videoPageWidgetInfo     = new QmlComponentInfo(tr("Video Stream"), QUrl::fromUserInput("qrc:/qml/VideoPageWidget.qml"));
#endif
        _p->healthPageWidgetInfo    = new QmlComponentInfo(tr("Health"),    QUrl::fromUserInput("qrc:/qml/HealthPageWidget.qml"));
        _p->vibrationPageWidgetInfo = new QmlComponentInfo(tr("Vibration"), QUrl::fromUserInput("qrc:/qml/VibrationPageWidget.qml"));

        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->valuesPageWidgetInfo));
        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->cameraPageWidgetInfo));
#if defined(QGC_GST_STREAMING)
        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->videoPageWidgetInfo));
#endif
        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->healthPageWidgetInfo));
        _p->instrumentPageWidgetList.append(QVariant::fromValue(_p->vibrationPageWidgetInfo));
    }
    return _p->instrumentPageWidgetList;
}

int QGCCorePlugin::defaultSettings()
{
    return 0;
}

QGCOptions* QGCCorePlugin::options()
{
    if(!_p->defaultOptions) {
        _p->defaultOptions = new QGCOptions();
    }
    return _p->defaultOptions;
}

bool QGCCorePlugin::overrideSettingsGroupVisibility(QString name)
{
    Q_UNUSED(name);

    // Always show all
    return true;
}

bool QGCCorePlugin::adjustSettingMetaData(const QString& settingsGroup, FactMetaData& metaData)
{
    if (settingsGroup != AppSettings::settingsGroup) {
        // All changes refer to AppSettings
        return true;
    }

    //-- Default Palette
    if (metaData.name() == AppSettings::indoorPaletteName) {
        QVariant outdoorPalette;
#if defined (__mobile__)
        outdoorPalette = 0;
#else
        outdoorPalette = 1;
#endif
        metaData.setRawDefaultValue(outdoorPalette);
        return true;
    //-- Auto Save Telemetry Logs
    } else if (metaData.name() == AppSettings::telemetrySaveName) {
#if defined (__mobile__)
        metaData.setRawDefaultValue(false);
        return true;
#else
        metaData.setRawDefaultValue(true);
        return true;
#endif
#if defined(__ios__)
    } else if (metaData.name() == AppSettings::savePathName) {
        QString appName = qgcApp()->applicationName();
        QDir rootDir = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        metaData.setRawDefaultValue(rootDir.filePath(appName));
        return false;
#endif
    }
    return true; // Show setting in ui
}

void QGCCorePlugin::setShowTouchAreas(bool show)
{
    if (show != _showTouchAreas) {
        _showTouchAreas = show;
        emit showTouchAreasChanged(show);
    }
}

void QGCCorePlugin::setShowAdvancedUI(bool show)
{
    if (show != _showAdvancedUI) {
        _showAdvancedUI = show;
        emit showAdvancedUIChanged(show);
    }
}

void QGCCorePlugin::paletteOverride(QString colorName, QGCPalette::PaletteColorInfo_t& colorInfo)
{
    Q_UNUSED(colorName);
    Q_UNUSED(colorInfo);
}

QString QGCCorePlugin::showAdvancedUIMessage(void) const
{
    return tr("WARNING: You are about to enter Advanced Mode. "
              "If used incorrectly, this may cause your vehicle to malfunction thus voiding your warranty. "
              "You should do so only if instructed by customer support. "
              "Are you sure you want to enable Advanced Mode?");
}

void QGCCorePlugin::valuesWidgetDefaultSettings(QStringList& largeValues, QStringList& smallValues)
{
    Q_UNUSED(smallValues);
    largeValues << "Vehicle.altitudeRelative" << "Vehicle.groundSpeed" << "Vehicle.flightTime";
}

QQmlApplicationEngine* QGCCorePlugin::createRootWindow(QObject *parent)
{
    QQmlApplicationEngine* pEngine = new QQmlApplicationEngine(parent);
    pEngine->addImportPath("qrc:/qml");
    pEngine->rootContext()->setContextProperty("joystickManager", qgcApp()->toolbox()->joystickManager());
    pEngine->rootContext()->setContextProperty("debugMessageModel", AppMessages::getModel());
    pEngine->load(QUrl(QStringLiteral("qrc:/qml/MainWindowNative.qml")));
    return pEngine;
}

bool QGCCorePlugin::mavlinkMessage(Vehicle* vehicle, LinkInterface* link, mavlink_message_t message)
{
    Q_UNUSED(vehicle);
    Q_UNUSED(link);
    Q_UNUSED(message);

    return true;
}

QmlObjectListModel* QGCCorePlugin::customMapItems(void)
{
    return &_p->_emptyCustomMapItems;
}

VideoReceiver* QGCCorePlugin::createVideoReceiver(QObject* parent)
{
    return new VideoReceiver(parent);
}

bool QGCCorePlugin::guidedActionsControllerLogging(void) const
{
    return GuidedActionsControllerLog().isDebugEnabled();
}

QString QGCCorePlugin::stableVersionCheckFileUrl(void) const
{
#ifdef QGC_CUSTOM_BUILD
    // Custom builds must override to turn on and provide their own location
    return QString();
#else
    return QString("https://s3-us-west-2.amazonaws.com/qgroundcontrol/latest/QGC.version.txt");
#endif
}

#if !defined(__mobile__)
QGCQmlWidgetHolder* QGCCorePlugin::createMainQmlWidgetHolder(QLayout *mainLayout, QWidget* parent)
{
    QGCQmlWidgetHolder* pMainQmlWidgetHolder = new QGCQmlWidgetHolder(QString(), nullptr, parent);
    mainLayout->addWidget(pMainQmlWidgetHolder);
    pMainQmlWidgetHolder->setVisible(true);
    QQmlEngine::setObjectOwnership(parent, QQmlEngine::CppOwnership);
    pMainQmlWidgetHolder->setContextPropertyObject("controller", parent);
    pMainQmlWidgetHolder->setContextPropertyObject("debugMessageModel", AppMessages::getModel());
    pMainQmlWidgetHolder->setSource(QUrl::fromUserInput("qrc:qml/MainWindowHybrid.qml"));
    return pMainQmlWidgetHolder;
}
#endif
