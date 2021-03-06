/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/

#include <config.h>

#import <Cocoa/Cocoa.h>

//#include <QtGui>
//#include <QMacNativeWidget>

#include <QApplication>
#include <QtCore>
#include <QIcon>
#include <extensionmanager.h>
#include <plexy.h>
#include <workspace.h>
#include <space.h>
#include <plexyconfig.h>
#include "desktopmanager.h"

#include <objc/objc.h>
#include <objc/message.h>

bool on_dock_clicked_func(id self,SEL _cmd,...)
{
  Q_UNUSED(self)
  Q_UNUSED(_cmd)

  [[NSWorkspace sharedWorkspace] hideOtherApplications];
  return YES;
}

class Runtime {
public:
  Runtime() {
    id cls = objc_getClass("NSApplication");
    SEL sharedApplication = sel_registerName("sharedApplication");
    id appInst = objc_msgSend(cls,sharedApplication);

    if(appInst != NULL)
    {
        id delegate = objc_msgSend(appInst, sel_registerName("delegate"));
        id delClass = objc_msgSend(delegate,  sel_registerName("class"));
        //const char* tst = class_getName(delClass->isa);
        bool classMethodFunc = class_addMethod(
                    (id)delClass,
                    sel_registerName(
                        "applicationShouldHandleReopen:hasVisibleWindows:"),
                    (IMP)on_dock_clicked_func,"B@:");

        if (!classMethodFunc) {
            NSLog(@"Failed to register with your mac");
        }
    }

    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath =
            CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char *pathPtr =
            CFStringGetCStringPtr(macPath,CFStringGetSystemEncoding());

    CFRelease(appUrlRef);
    CFRelease(macPath);

    UIKit::ExtensionManager *loader =
            UIKit::ExtensionManager::init(
                QDir::toNativeSeparators(QLatin1String(pathPtr)  +
                                         QLatin1String(
                                             "/Contents/PlugIns/plexydesk/data/"
                                             )),
                QDir::toNativeSeparators(QLatin1String(pathPtr)  +
                                         QLatin1String(
                                             "/Contents/PlugIns/plexydesk/")));
    Q_UNUSED(loader);

    int screen_count = [[NSScreen screens] count];


    for(int i = 0; i < screen_count; i++) {
      qDebug() << Q_FUNC_INFO << "Screen ID :" << i;
      DesktopManager *workspace = new DesktopManager();
      m_workspace_list.push_back(workspace);

      workspace->set_accelerated_rendering(false);
      workspace->move_to_screen(i);
      workspace->add_default_controller("classicbackdrop");
      workspace->add_default_controller("dockwidget");
      workspace->add_default_controller("plexyclock");
      workspace->add_default_controller("desktopnoteswidget");
      workspace->add_default_controller("folderwidget");
      workspace->add_default_controller("photoframe");

      workspace->restore_session();

      if (workspace->space_count() <= 0) {
        workspace->add_default_space();
      }

      NSView *_desktopView = reinterpret_cast<NSView *>(workspace->winId());

      [[_desktopView window]
              setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces];
      [[_desktopView window] setHasShadow:NO];
      [[_desktopView window] setOpaque:NO];
      [[_desktopView window] setLevel:kCGDesktopIconWindowLevel + 1];
      [[_desktopView window] makeKeyAndOrderFront:_desktopView];
      [[_desktopView window] setBackgroundColor:[NSColor clearColor]];

      workspace->show();
    }
  }

  ~Runtime() {
    std::for_each(std::begin(m_workspace_list), std::end(m_workspace_list),
                  [&] (DesktopManager *a_manager) {
                  QString::fromStdString(a_manager->workspace_instance_name());
         delete a_manager;
    });

  }

private:
    std::vector<DesktopManager*> m_workspace_list;
};

int main(int argc, char *argv[])
{
  QApplication qtApp(argc, argv);

  Runtime runtime;

  qtApp.setStyleSheet(
        QString("QScrollBar:vertical{ border: 2px solid grey;"
                        " background: #32CC99; height: 15px; margin: 0px 20px"
                        " 0 20px; }"));
  return qtApp.exec();
}
