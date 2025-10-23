/*
 * Copyright (C) 2005-2012 University of Muenster, Germany.
 * Visualization and Computer Graphics Group <http://viscg.uni-muenster.de>
 * For a list of authors please refer to the file "CREDITS.txt".
 * Copyright (C) 2012-2013 Korea Institiute of Science and Technologhy, Seoul.
 * Linqing Feng, Jinny Kim's lab <http://jinny.kist.re.kr>
 *
 * This file is derived from code of the free Voreen software package.
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2 as published by the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License in the file
 * "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>.
 */

#include "z3dprocessor.h"
#include "QsLog.h"
#include "z3dapplication.h"
#include "z3dport.h"
#include "z3dinteractionhandler.h"
#include "zparameter.h"
#include "zeventlistenerparameter.h"

#include <cassert>

Z3DProcessor::Z3DProcessor(const QString &name)
  : QObject()
  , m_initialized(false)
  , m_invalidationState(InvalidAllResult)
  , m_name(name)
  , m_invalidationVisited(false)
{
}

Z3DProcessor::~Z3DProcessor()
{
  if (isInitialized()) {
    LWARN() << getClassName() << "has not been deinitialized";
    try {
      deinitialize();
    }
    catch (...) {}
  }
}

void Z3DProcessor::initialize()
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::initialize start");
  if (!Z3DApplication::app()) {
    LERROR() << "3D Application not instantiated";
    throw Exception("3D Application not instantiated");
  }

  if (isInitialized()) {
    LWARN() << getClassName() << "already initialized";
    return;
  }

  // initialize ports
  for (size_t i=0; i < m_inputPorts.size(); ++i) {
    if (!m_inputPorts[i]->isInitialized())
      m_inputPorts[i]->initialize();
  }
  for (size_t i=0; i < m_outputPorts.size(); ++i) {
    if (!m_outputPorts[i]->isInitialized())
      m_outputPorts[i]->initialize();
  }
  CHECK_GL_ERROR_NOTE("Z3DProcessor::initialize end");
}

void Z3DProcessor::deinitialize()
{
  if (!isInitialized()) {
    LWARN() << getClassName() << "not initialized";
    return;
  }
  // deinitialize ports
  for (size_t i=0; i < m_inputPorts.size(); ++i) {
    m_inputPorts[i]->deinitialize();
  }
  for (size_t i=0; i < m_outputPorts.size(); ++i) {
    m_outputPorts[i]->deinitialize();
  }
  CHECK_GL_ERROR;
}

void Z3DProcessor::addPort(Z3DInputPortBase *port)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addPort Z3DInputPortBase start");
  assert(port);
  port->setProcessor(this);

  m_inputPorts.push_back(port);

  std::map<QString, Z3DInputPortBase*>::const_iterator it = m_inputPortMap.find(port->getName());
  if (it == m_inputPortMap.end())
    m_inputPortMap.insert(std::make_pair(port->getName(), port));
  else {
    LERROR() << getClassName() << "port" << port->getName() << "has already been inserted!";
    assert(false);
  }
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addPort Z3DInputPortBase end");
}

void Z3DProcessor::addPort(Z3DOutputPortBase *port)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addPort Z3DOutputPortBase start");
  assert(port);
  port->setProcessor(this);
  m_outputPorts.push_back(port);
  std::map<QString, Z3DOutputPortBase*>::const_iterator it = m_outputPortMap.find(port->getName());
  if (it == m_outputPortMap.end())
    m_outputPortMap.insert(std::make_pair(port->getName(), port));
  else {
    LERROR() << getClassName() << "port" << port->getName() << "has already been inserted!";
    assert(false);
  }
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addPort Z3DOutputPortBase end");
}

void Z3DProcessor::removePort(Z3DInputPortBase* port)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::removePort Z3DInputPortBase start");
  assert(port);
  if (port->isInitialized()) {
    LWARN() << getClassName() << "port" << port->getName()
             << "has not been deinitialized";
  }
  m_inputPorts.erase(std::find(m_inputPorts.begin(), m_inputPorts.end(), port));
  std::map<QString, Z3DInputPortBase*>::iterator inIt = m_inputPortMap.find(port->getName());
  if (inIt != m_inputPortMap.end())
    m_inputPortMap.erase(inIt);
  else {
    LERROR() << getClassName() << "port" << port->getName() << "was not found!";
    assert(false);
  }
  CHECK_GL_ERROR_NOTE("Z3DProcessor::removePort Z3DInputPortBase end");
}

void Z3DProcessor::removePort(Z3DOutputPortBase* port)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::removePort Z3DOutputPortBase start");
  assert(port);
  if (port->isInitialized()) {
    LWARN() << getClassName() << "port" << port->getName()
             << "has not been deinitialized";
  }
  m_outputPorts.erase(std::find(m_outputPorts.begin(), m_outputPorts.end(), port));
  std::map<QString, Z3DOutputPortBase*>::iterator outIt = m_outputPortMap.find(port->getName());
  if (outIt != m_outputPortMap.end())
    m_outputPortMap.erase(outIt);
  else {
    LERROR() << getClassName() << "port" << port->getName() << "was not found!";
    assert(false);
  }
  CHECK_GL_ERROR_NOTE("Z3DProcessor::removePort Z3DOutputPortBase end");
}

void Z3DProcessor::addParameter(ZParameter *para, InvalidationState inv)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addParameter start");
  assert(para);
  m_parameters.push_back(para);
  if (inv != Valid) {
    connect(para, SIGNAL(valueChanged()), this, SLOT(invalidateResult()));
  }
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addParameter end");
}

void Z3DProcessor::removeParameter(ZParameter *para)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::removeParameter start");
  assert(para);
  if (!getParameter(para->getName())) {
    LERROR() << getClassName() << "parameter" << para->getName() << "cannot be removed, it does not exist";
  } else {
    para->disconnect(this);
    m_parameters.erase(std::find(m_parameters.begin(), m_parameters.end(), para));
  }
  CHECK_GL_ERROR_NOTE("Z3DProcessor::removeParameter end");
}

ZParameter* Z3DProcessor::getParameter(const QString &name) const
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::getParameter start");
  for (size_t i=0; i<m_parameters.size(); i++) {
    if (m_parameters[i]->getName() == name)
      return m_parameters[i];
  }
  return NULL;
}

bool Z3DProcessor::isInInteractionMode() const
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::isInInteractionMode start");
  return (!m_interactionModeSources.empty());
}

Z3DInputPortBase *Z3DProcessor::getInputPort(const QString &name) const
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::getInputPort start");
  for (size_t i=0; i < m_inputPorts.size(); i++) {
    if (m_inputPorts[i]->getName() == name)
      return m_inputPorts[i];
  }
  return NULL;
}

Z3DOutputPortBase *Z3DProcessor::getOutputPort(const QString &name) const
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::getOutputPort start");
  for (size_t i=0; i < m_outputPorts.size(); i++) {
    if (m_outputPorts[i]->getName() == name)
      return m_outputPorts[i];
  }
  return NULL;
}

void Z3DProcessor::invalidate(InvalidationState inv)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::invalidate start");
  m_invalidationState |= inv;
  if (inv == Z3DProcessor::Valid) return;
  if (!isInitialized()) return;

  if (!m_invalidationVisited) {
    m_invalidationVisited = true;
    for (size_t i=0; i<m_outputPorts.size(); ++i) m_outputPorts[i]->invalidate();
    m_invalidationVisited = false;
  }
  CHECK_GL_ERROR_NOTE("Z3DProcessor::invalidate end");
}

bool Z3DProcessor::isReady(Z3DEye) const
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::isReady start");
  if (!isInitialized())
    return false;

  for(size_t i=0; i<m_inputPorts.size(); ++i)
    if (!m_inputPorts[i]->isReady())
      return false;

  for (size_t i=0; i<m_outputPorts.size(); ++i)
    if(!m_outputPorts[i]->isReady())
      return false;

  return true;
}

void Z3DProcessor::toggleInteractionMode(bool interactionMode, void* source)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::toggleInteractionMode start");
  if (interactionMode) {
    if (m_interactionModeSources.find(source) == m_interactionModeSources.end()) {
      m_interactionModeSources.insert(source);
      if (m_interactionModeSources.size() == 1)
        enterInteractionMode();
    }
  } else {
    if (m_interactionModeSources.find(source) != m_interactionModeSources.end()) {
      m_interactionModeSources.erase(source);
      if (m_interactionModeSources.empty())
        exitInteractionMode();
    }
  }
CHECK_GL_ERROR_NOTE("Z3DProcessor::toggleInteractionMode end");
}

void Z3DProcessor::setValid(Z3DEye eye)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::setValid start");
  if (eye == CenterEye) m_invalidationState &= ~InvalidMonoViewResult;
  else if (eye == LeftEye) m_invalidationState &= ~InvalidLeftEyeResult;
  else m_invalidationState &= ~InvalidRightEyeResult;
  for (size_t i=0; i<m_inputPorts.size(); ++i) m_inputPorts[i]->setValid();
  CHECK_GL_ERROR_NOTE("Z3DProcessor::setValid end");
}

bool Z3DProcessor::isValid(Z3DEye eye) const
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::isValid start");
  if (eye == CenterEye)
    return !m_invalidationState.testFlag(InvalidMonoViewResult);
  else if (eye == LeftEye)
    return !m_invalidationState.testFlag(InvalidLeftEyeResult);
  else
    return !m_invalidationState.testFlag(InvalidRightEyeResult);
  CHECK_GL_ERROR_NOTE("Z3DProcessor::isValid end");
}

void Z3DProcessor::addEventListener(ZEventListenerParameter* para)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addEventListener start");
  assert(para);
  addParameter(para);
  m_eventListeners.push_back(para);
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addEventListener end");
}

void Z3DProcessor::addInteractionHandler(Z3DInteractionHandler* handler)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::addInteractionHandler start");
  assert(handler);
  m_interactionHandlers.push_back(handler);
CHECK_GL_ERROR_NOTE("Z3DProcessor::addInteractionHandler end");
}

void Z3DProcessor::onEvent(QEvent *e, int w, int h)
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::onEvent start");
  e->ignore();
  // propagate to interaction handlers
  for (size_t i=0; i<m_interactionHandlers.size() && !e->isAccepted(); ++i) {
    for (size_t j=0; j<m_interactionHandlers[i]->getEventListeners().size() && !e->isAccepted(); ++j) {
      m_interactionHandlers[i]->getEventListeners().at(j)->sendEvent(e, w, h);
    }
  }
  // propagate to event listeners
  for (size_t i = 0; (i < m_eventListeners.size()) && !e->isAccepted(); ++i)
    m_eventListeners[i]->sendEvent(e, w, h);
CHECK_GL_ERROR_NOTE("Z3DProcessor::onEvent end");
}

void Z3DProcessor::disconnectAllPorts()
{
  CHECK_GL_ERROR_NOTE("Z3DProcessor::disconnectAllPorts start");
  for (size_t i = 0; i < m_inputPorts.size(); ++i) {
    m_inputPorts[i]->disconnectAll();
  }

  for (size_t i = 0; i < m_outputPorts.size(); ++i) {
    m_outputPorts[i]->disconnectAll();
  }
CHECK_GL_ERROR_NOTE("Z3DProcessor::disconnectAllPorts end");
}
