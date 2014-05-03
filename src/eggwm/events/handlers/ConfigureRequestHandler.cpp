/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/handlers/ConfigureRequestHandler.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  ConfigureRequestHandler
 * @author José Expósito
 */
#include "ConfigureRequestHandler.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

ConfigureRequestHandler::ConfigureRequestHandler(XWindowList* wl)
        : EventHandler(wl) {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool ConfigureRequestHandler::processEvent(XEvent* event) {
    Window windowID = event->xconfigurerequest.window;
    qDebug() << "[+] ConfigureRequest event 0x" << hex << windowID;

    // Si la ventana es un cliente
    if(this->wl->existClient(windowID)) {
        qDebug() << "\tLa ventana es un cliente, configurándolo...";
        XWindow* xwindow = this->wl->getXWindowByClientID(windowID);

        // Si el cliente bypassea el WM
        if(xwindow->bypassWM()) {
            qDebug() << "\tEl cliente bypassea el WM";
            return false;

        // Si no lo bypassea
        } else {
            if(event->xconfigurerequest.value_mask & CWX)
                xwindow->setX(event->xconfigurerequest.x);

            if(event->xconfigurerequest.value_mask & CWY)
                xwindow->setY(event->xconfigurerequest.y);

            // Hay que comprobar si la ventana tiene o no marco porque si no
            // las ventanas que solo aceptan tamaños múltiplos de un cierto
            // número dan problemas

            Config* cfg = Config::getInstance();

            if(event->xconfigurerequest.value_mask & CWWidth) {
                if(xwindow->haveFrame())
                    xwindow->setWidth(event->xconfigurerequest.width
                            + cfg->getLeftBorderWidth()
                            + cfg->getRightBorderWidth());
                else
                    xwindow->setWidth(event->xconfigurerequest.width);
            }

            if(event->xconfigurerequest.value_mask & CWHeight) {
                if(xwindow->haveFrame())
                    xwindow->setHeight(event->xconfigurerequest.height
                            + cfg->getTitlebarWidth() + cfg->getTopBorderWidth()
                            + cfg->getBottomBorderWidth());
                else
                    xwindow->setHeight(event->xconfigurerequest.height);
            }

            return true;
        }

    // Si no es un cliente
    } else {
        qDebug() << "\tLa ventana no es un cliente";
        return false;
    }
}

#if QT_VERSION >= 0x050000
bool ConfigureRequestHandler::processEvent(xcb_generic_event_t* event) 
{
    xcb_configure_request_event_t *configure = reinterpret_cast<xcb_configure_request_event_t *>(event);
    Window windowID = configure->window;
    if (this->wl->existClient(windowID)) {
        XWindow* xwindow = this->wl->getXWindowByClientID(windowID);
        if (!xwindow->bypassWM()) {
            if (configure->value_mask & CWX) xwindow->setX(configure->x);
            if (configure->value_mask & CWY) xwindow->setY(configure->y);
            Config* cfg = Config::getInstance();
            if (configure->value_mask & CWWidth) {
                if (xwindow->haveFrame()) {
                    xwindow->setWidth(
                        configure->width + cfg->getLeftBorderWidth() + 
                        cfg->getRightBorderWidth());
                } else xwindow->setWidth(configure->width);
            }
            if (configure->value_mask & CWHeight) {
                if(xwindow->haveFrame()) {
                    xwindow->setHeight(
                        configure->height + cfg->getTitlebarWidth() + 
                        cfg->getTopBorderWidth() + cfg->getBottomBorderWidth());
                } else xwindow->setHeight(configure->height);
            }
            return true;
        }
    }
    return false;
}
#endif
