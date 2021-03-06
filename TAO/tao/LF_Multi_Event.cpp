#include "tao/LF_Multi_Event.h"
#include "tao/Connection_Handler.h"
#include "ace/OS_Memory.h"

TAO_BEGIN_VERSIONED_NAMESPACE_DECL


TAO_LF_Multi_Event::TAO_LF_Multi_Event ()
  : TAO_LF_Event (),
    events_ (nullptr),
    winner_ (nullptr)
{
}

TAO_LF_Multi_Event::~TAO_LF_Multi_Event ()
{
  while (this->events_ != nullptr)
    {
      Event_Node *n = this->events_->next_;
      delete this->events_;
      this->events_ = n;
    }
}

int
TAO_LF_Multi_Event::bind (TAO_LF_Follower *follower)
{
  if (this->TAO_LF_Event::bind(follower) == -1)
    {
      return -1;
    }

  for (Event_Node *n = this->events_; n != nullptr; n = n->next_)
    if (n->ptr_->bind(follower) == -1)
      {
        return -1;
      }
  return 0;
}

int
TAO_LF_Multi_Event::unbind (TAO_LF_Follower *follower)
{
  if (this->TAO_LF_Event::unbind (follower) == -1)
    {
      return -1;
    }

  for (Event_Node *n = this->events_; n != nullptr; n = n->next_)
    if (n->ptr_->unbind (follower) == -1)
      {
        return -1;
      }
  return 0;
}


void
TAO_LF_Multi_Event::add_event (TAO_Connection_Handler *ev)
{
  Event_Node *node = nullptr;
  ACE_NEW (node, Event_Node);
  node->next_ = this->events_;
  node->ptr_ = ev;

  this->events_ = node;
}

TAO_Connection_Handler*
TAO_LF_Multi_Event::winner ()
{
  return this->winner_;
}

TAO_Transport *
TAO_LF_Multi_Event::base_transport ()
{
  return (this->events_ == nullptr) ? nullptr : this->events_->ptr_->transport();
}

bool
TAO_LF_Multi_Event::successful_i () const
{
  for (Event_Node *n = this->events_; n != nullptr; n = n->next_)
    if (n->ptr_->successful_i())
      {
        this->winner_ = n->ptr_;
        return true;
      }
  return false;
}

bool
TAO_LF_Multi_Event::error_detected_i () const
{
  for (Event_Node *n = this->events_; n != nullptr; n = n->next_)
    {
      if (!n->ptr_->error_detected_i ())
        {
          return false;
        }
    }
  return true;
}

void
TAO_LF_Multi_Event::state_changed_i (LFS_STATE )
{
}

bool
TAO_LF_Multi_Event::is_state_final () const
{
  for (Event_Node *n = this->events_; n != nullptr; n = n->next_)
    {
      if (!n->ptr_->is_state_final ())
        {
          return false;
        }
    }
  return true;
}

TAO_END_VERSIONED_NAMESPACE_DECL
