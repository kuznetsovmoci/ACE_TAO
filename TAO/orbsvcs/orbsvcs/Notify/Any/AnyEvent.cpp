// $Id$

#include "AnyEvent.h"

ACE_RCSID (Notify, TAO_Notify_AnyEvent, "$Id$")

#include "../Consumer.h"
#include "tao/debug.h"

//#define DEBUG_LEVEL 10
#ifndef DEBUG_LEVEL
# define DEBUG_LEVEL TAO_debug_level
#endif //DEBUG_LEVEL

TAO_Notify_EventType TAO_Notify_AnyEvent_No_Copy::event_type_;

TAO_Notify_AnyEvent_No_Copy::TAO_Notify_AnyEvent_No_Copy (const CORBA::Any &event)
  : event_ (&event)
{
}

TAO_Notify_AnyEvent_No_Copy::~TAO_Notify_AnyEvent_No_Copy ()
{
}

const TAO_Notify_EventType&
TAO_Notify_AnyEvent_No_Copy::type (void) const
{
  return this->event_type_;
}

void
TAO_Notify_AnyEvent_No_Copy::convert (CosNotification::StructuredEvent& notification) const
{
  TAO_Notify_Event::translate (*this->event_, notification);
}

CORBA::Boolean
TAO_Notify_AnyEvent_No_Copy::do_match (CosNotifyFilter::Filter_ptr filter ACE_ENV_ARG_DECL) const
{
  if (DEBUG_LEVEL > 0)
    ACE_DEBUG ((LM_DEBUG, "Notify (%P|%t) - "
                "TAO_Notify_AnyEvent::do_match ()\n"));

  return filter->match(*this->event_ ACE_ENV_ARG_PARAMETER);
}

void
TAO_Notify_AnyEvent_No_Copy::push (TAO_Notify_Consumer* consumer ACE_ENV_ARG_DECL) const
{
  if (DEBUG_LEVEL > 0)
    ACE_DEBUG ((LM_DEBUG, "Notify (%P|%t) - "
                "TAO_Notify_AnyEvent::push \n"));

  consumer->push (*this->event_ ACE_ENV_ARG_PARAMETER);
}

void
TAO_Notify_AnyEvent_No_Copy::push (Event_Forwarder::StructuredProxyPushSupplier_ptr forwarder ACE_ENV_ARG_DECL) const
{
  CosNotification::StructuredEvent notification;

  TAO_Notify_Event::translate (*this->event_, notification);

  forwarder->forward_structured (notification ACE_ENV_ARG_PARAMETER);
}

void
TAO_Notify_AnyEvent_No_Copy::push_no_filtering (Event_Forwarder::StructuredProxyPushSupplier_ptr forwarder ACE_ENV_ARG_DECL) const
{
  CosNotification::StructuredEvent notification;

  TAO_Notify_Event::translate (*this->event_, notification);

  forwarder->forward_structured_no_filtering (notification ACE_ENV_ARG_PARAMETER);
}

void
TAO_Notify_AnyEvent_No_Copy::push (Event_Forwarder::ProxyPushSupplier_ptr forwarder ACE_ENV_ARG_DECL) const
{
  forwarder->forward_any (*this->event_ ACE_ENV_ARG_PARAMETER);
}

void
TAO_Notify_AnyEvent_No_Copy::push_no_filtering (Event_Forwarder::ProxyPushSupplier_ptr forwarder ACE_ENV_ARG_DECL) const
{
  forwarder->forward_any_no_filtering (*this->event_ ACE_ENV_ARG_PARAMETER);
}

void
TAO_Notify_AnyEvent_No_Copy::marshal (TAO_OutputCDR & cdr) const
{
  static const ACE_CDR::Octet ANY_CODE = MARSHAL_ANY;
  cdr.write_octet (ANY_CODE);
  cdr << (*this->event_);
}

//static
TAO_Notify_AnyEvent *
TAO_Notify_AnyEvent_No_Copy::unmarshal (TAO_InputCDR & cdr)
{
  TAO_Notify_AnyEvent * event = 0;
  CORBA::Any body;
  if (cdr >> body)
  {
    event = new TAO_Notify_AnyEvent (body);
  }
  return event;
}

TAO_Notify_Event *
TAO_Notify_AnyEvent_No_Copy::copy (ACE_ENV_SINGLE_ARG_DECL) const
{
  TAO_Notify_Event * new_event;
  ACE_NEW_THROW_EX (new_event,
                    TAO_Notify_AnyEvent (*this->event_),
                    CORBA::NO_MEMORY ());
  ACE_CHECK_RETURN (0);
  return new_event;
}


/*****************************************************************************************************/

TAO_Notify_AnyEvent::TAO_Notify_AnyEvent (const CORBA::Any &event)
  : TAO_Notify_AnyEvent_No_Copy (event)
    , any_copy_ (event)
{
  this->event_ = &any_copy_;
}

TAO_Notify_AnyEvent::~TAO_Notify_AnyEvent ()
{
}
