#include "shortcutmanager.h"
#include "qvapplication.h"
#include "actionmanager.h"

#include <QSettings>

ShortcutManager::ShortcutManager(QObject *parent) : QObject(parent)
{
    initializeShortcutsList();
    updateShortcuts();
}

void ShortcutManager::updateShortcuts()
{
    QSettings settings;
    settings.beginGroup("shortcuts");

    // Set all shortcuts to the user-set shortcut or the default
    for (auto &shortcut : shortcutsList)
    {
        shortcut.shortcuts = settings.value(shortcut.name, shortcut.defaultShortcuts).toStringList();
    }

    // Set all action shortcuts now that the shortcuts have changed
    const auto &actionManager = qvApp->getActionManager();
    for (const auto &shortcut : getShortcutsList())
    {
        const auto actionList = actionManager.getAllInstancesOfAction(shortcut.name);

        for (const auto &action : actionList)
        {
            if (action)
                action->setShortcuts(stringListToKeySequenceList(shortcut.shortcuts));
        }
    }

    emit shortcutsUpdated();
}

void ShortcutManager::initializeShortcutsList()
{
    shortcutsList.append({tr("Open"), "open", keyBindingsToStringList(QKeySequence::Open), {}});
    shortcutsList.append({tr("Open URL"), "openurl", QStringList(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O).toString()), {}});
    shortcutsList.append({tr("Open Containing Folder"), "opencontainingfolder", {}, {}});
    //Sets open containing folder action name to platform-appropriate alternative
#ifdef Q_OS_WIN
    shortcutsList.last().readableName = tr("Show in Explorer");
#elif defined Q_OS_MACOS
    shortcutsList.last().readableName  = tr("Show in Finder");
#endif
    shortcutsList.append({tr("Show File Info"), "showfileinfo", QStringList(QKeySequence(Qt::Key_I).toString()), {}});

	shortcutsList.append({tr("Copy"), "copy", keyBindingsToStringList(QKeySequence::Copy), {}});
    shortcutsList.append({tr("Paste"), "paste", keyBindingsToStringList(QKeySequence::Paste), {}});
	shortcutsList.append({tr("Delete"), "remove", QStringList({QKeySequence(Qt::ALT + Qt::Key_Delete).toString()}), {}});
    shortcutsList.append({tr("Rename"), "rename", QStringList({QKeySequence(Qt::Key_F2).toString(), QKeySequence(Qt::CTRL + Qt::Key_R).toString()}), {}});

	//  Rate
	shortcutsList.append({tr("Rating Front Increase"), "rateFrontInc", QStringList(QKeySequence(Qt::ALT + Qt::Key_3).toString()), {}});
	shortcutsList.append({tr("Rating Front Decrease"), "rateFrontDec", QStringList(QKeySequence(Qt::ALT + Qt::Key_4).toString()), {}});
	shortcutsList.append({tr("Rating End Add"), "rateEndAdd", QStringList(QKeySequence(Qt::ALT + Qt::Key_1).toString()), {}});
	shortcutsList.append({tr("Rating End Remove"), "rateEndRemove", QStringList(QKeySequence(Qt::ALT + Qt::Key_2).toString()), {}});

	shortcutsList.append({tr("First File"), "firstFile", QStringList(QKeySequence(Qt::Key_Home).toString()), {}});
	shortcutsList.append({tr("Last File"), "lastFile", QStringList(QKeySequence(Qt::Key_End).toString()), {}});
    shortcutsList.append({tr("Previous File"), "previousFile", QStringList(QKeySequence(Qt::Key_Left).toString()), {}});
    shortcutsList.append({tr("Next File"), "nextFile", QStringList(QKeySequence(Qt::Key_Right).toString()), {}});

	//  Skip
	shortcutsList.append({tr("Previous File Skip"), "previousFileSkip", QStringList(QKeySequence(Qt::Key_PageUp).toString()), {}});
    shortcutsList.append({tr("Next File Skip"), "nextFileSkip", QStringList(QKeySequence(Qt::Key_PageDown).toString()), {}});
	shortcutsList.append({tr("Previous File Rated"), "previousFileRate", QStringList(QKeySequence(Qt::Key_Up).toString()), {}});
    shortcutsList.append({tr("Next File Rated"), "nextFileRate", QStringList(QKeySequence(Qt::Key_Down).toString()), {}});

	shortcutsList.append({tr("Zoom In"), "zoomin", keyBindingsToStringList(QKeySequence::ZoomIn), {}});
    // Allow zooming with Ctrl + plus like a regular person (without holding shift)
    if (!shortcutsList.last().defaultShortcuts.contains(QKeySequence(Qt::CTRL + Qt::Key_Equal).toString()))
        shortcutsList.last().defaultShortcuts << QKeySequence(Qt::CTRL + Qt::Key_Equal).toString();
    shortcutsList.append({tr("Zoom Out"), "zoomout", keyBindingsToStringList(QKeySequence::ZoomOut), {}});
    shortcutsList.append({tr("Reset Zoom"), "resetzoom", QStringList(QKeySequence(Qt::CTRL + Qt::Key_0).toString()), {}});
    shortcutsList.append({tr("Original Size"), "originalsize", QStringList(QKeySequence(Qt::Key_O).toString()), {}});
    
	shortcutsList.append({tr("Rotate Right"), "rotateright", QStringList(QKeySequence(Qt::Key_R).toString()), {}});
    shortcutsList.append({tr("Rotate Left"), "rotateleft", QStringList(QKeySequence(Qt::Key_T).toString()), {}});
    shortcutsList.append({tr("Mirror"), "mirror", QStringList(QKeySequence(Qt::Key_F).toString()), {}});
    shortcutsList.append({tr("Flip"), "flip", QStringList(QKeySequence(Qt::CTRL + Qt::Key_F).toString()), {}});

	shortcutsList.append({tr("Full Screen"), "fullscreen", keyBindingsToStringList(QKeySequence::FullScreen), {}});
    //Fixes alt+enter only working with numpad enter when using qt's standard keybinds
#ifdef Q_OS_WIN
    shortcutsList.last().defaultShortcuts << QKeySequence(Qt::ALT + Qt::Key_Return).toString();
#elif defined Q_OS_UNIX & !defined Q_OS_MACOS
    // F11 is for some reason not there by default in GNOME
    if (shortcutsList.last().defaultShortcuts.contains(QKeySequence(Qt::CTRL + Qt::Key_F11).toString()) &&
        !shortcutsList.last().defaultShortcuts.contains(QKeySequence(Qt::Key_F11).toString()))
    {
        shortcutsList.last().defaultShortcuts << QKeySequence(Qt::Key_F11).toString();
    }
#endif
    shortcutsList.append({tr("Save Frame As"), "saveframeas", keyBindingsToStringList(QKeySequence::Save), {}});

	shortcutsList.append({tr("Pause"), "pause", QStringList(QKeySequence(Qt::Key_P).toString()), {}});
    shortcutsList.append({tr("Next Frame"), "nextframe", QStringList(QKeySequence(Qt::Key_N).toString()), {}});
    shortcutsList.append({tr("Decrease Speed"), "decreasespeed", QStringList(QKeySequence(Qt::Key_BracketLeft).toString()), {}});
    shortcutsList.append({tr("Reset Speed"), "resetspeed", QStringList(QKeySequence(Qt::Key_Backslash).toString()), {}});
    shortcutsList.append({tr("Increase Speed"), "increasespeed", QStringList(QKeySequence(Qt::Key_BracketRight).toString()), {}});

	shortcutsList.append({tr("Toggle Slideshow"), "slideshow", {}, {}});
    shortcutsList.append({tr("Options"), "options", keyBindingsToStringList(QKeySequence::Preferences), {}});
#ifdef Q_OS_UNIX
    shortcutsList.last().readableName = tr("Preferences");
#endif
    // mac exclusive shortcuts
#ifdef Q_OS_MACOS
    shortcutsList.append({tr("New Window"), "newwindow", keyBindingsToStringList(QKeySequence::New), {}});
    shortcutsList.append({tr("Close Window"), "closewindow", QStringList(QKeySequence(Qt::CTRL + Qt::Key_W).toString()), {}});
    shortcutsList.append({tr("Close All"), "closeallwindows", QStringList(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_W).toString()), {}});
#endif
    shortcutsList.append({tr("Quit"), "quit", keyBindingsToStringList(QKeySequence::Quit), {}});
#ifndef Q_OS_MACOS
    shortcutsList.last().defaultShortcuts << QKeySequence(Qt::CTRL + Qt::Key_W).toString();
#endif
#ifdef Q_OS_WIN
    shortcutsList.last().readableName = tr("Exit");
#endif
}
