/*
 Copyright (C) %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 2 of
 the License or (at your option) version 3 or any later version
 accepted by the membership of KDE e.V. (or its successor approved
 by the membership of KDE e.V.), which shall act as a proxy 
 defined in Section 14 of version 3 of the license.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "drmresolutionwindow.h"

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QDebug>

extern "C" {
#include <xf86drmMode.h>
}

#include "logind.h"
#include "udev.h"
#include "drm_pointer.h"

using namespace KWin;

int main(int argc, char **argv)
{
    QGuiApplication application(argc, argv);

    QCommandLineParser parser;
    parser.process(application);

    drmresolutionWindow *mainWindow = new drmresolutionWindow;

    auto m_udev = new Udev;
    auto device = m_udev->primaryGpu();
    qDebug() << "gpu" << device->devNode();

    QObject::connect(LogindIntegration::self(), &LogindIntegration::connectedChanged, LogindIntegration::self(), [&device]() {
        LogindIntegration::self()->takeControl();
        const int fd = LogindIntegration::self()->takeDevice(device->devNode());
        qDebug() << "fd" << fd;
        LogindIntegration::self()->releaseControl();

        KWin::ScopedDrmPointer<_drmModeRes, &drmModeFreeResources> resources(drmModeGetResources(fd));
        drmModeRes *res = resources.data();
        if (!resources) {
            qWarning() << "drmModeGetResources failed";
            return;
        }

        for (int i = 0; i < res->count_connectors; ++i) {
            const auto conid = res->connectors[i];
            ScopedDrmPointer<_drmModeConnector, &drmModeFreeConnector> connector(drmModeGetConnector(fd, conid));
            if (!connector) {
                return;
            }
            for (int i = 0; i < connector->count_props; ++i) {
                ScopedDrmPointer<_drmModeProperty, &drmModeFreeProperty> property(drmModeGetProperty(fd, connector->props[i]));
                if (!property) {
                    continue;
                }
                qDebug() << "xxxx" << property->flags << property->name /*<< property->values[i]*/;
            }

        }

        drmModeGetProperty(fd, 0);
    });
    return application.exec();
}
