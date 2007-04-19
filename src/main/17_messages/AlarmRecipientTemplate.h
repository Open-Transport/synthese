
/** AlarmRecipientTemplate class header.
    @file AlarmRecipientTemplate.h

    This file belongs to the SYNTHESE project (public transportation specialized software)
    Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_AlarmRecipientTemplate_h__
#define SYNTHESE_AlarmRecipientTemplate_h__

#include <set>
#include <map>

#include "17_messages/AlarmRecipient.h"
#include "17_messages/SentAlarm.h"

#include "04_time/DateTime.h"

namespace synthese
{
    namespace messages
    {
	/** AlarmRecipientTemplate class.
	    @ingroup m17
	*/
	template<class T>
	    class AlarmRecipientTemplate : public AlarmRecipient
	{
	public:
	    typedef typename std::map<const SentAlarm*, std::set<const T*> >	LinksSetAlarm;
	    typedef typename std::map<const T*, std::set<const SentAlarm*> >	LinksSetObject;

	private:
	    static LinksSetAlarm _linksAlarm;
	    static LinksSetObject _linksObject;

	protected:
	    static void add(const T* object, const SentAlarm* alarm);
	    static void remove(const T* object, const SentAlarm* alarm);
			
	public:
	    AlarmRecipientTemplate(const std::string& title);

	    static std::set<const SentAlarm*>	getLinkedAlarms(const T* object);
	    static std::set<const T*>			getLinkedObjects(const SentAlarm* alarm);

	    static const SentAlarm* getAlarm(const T* object);
	    static const SentAlarm* getAlarm(const T* object, const time::DateTime& date);
	};

	template<class T>
	    const SentAlarm* synthese::messages::AlarmRecipientTemplate<T>::getAlarm( const T* object, const time::DateTime& date )
	{
	    typename LinksSetObject::iterator it = _linksObject.find(object);
	    if (it == _linksObject.end())
			return NULL;

	    const SentAlarm* alarm = NULL;
	    for (std::set<const SentAlarm*>::const_iterator its = it->second.begin(); its != it->second.end(); ++its)
	    {
			const SentAlarm* candidateAlarm = *its;
			if (candidateAlarm->isApplicable(date) && (
				alarm == NULL ||
				candidateAlarm->getLevel() > alarm->getLevel() ||
				candidateAlarm->getLevel() == alarm->getLevel() &&
				!candidateAlarm->getPeriodStart().isUnknown() &&
				candidateAlarm->getPeriodStart() > alarm->getPeriodStart()
				)) alarm = candidateAlarm;

	    }
	    return alarm;
	}

	template<class T>
	    const SentAlarm* synthese::messages::AlarmRecipientTemplate<T>::getAlarm( const T* object )
	{
	    return AlarmRecipientTemplate<T>::getAlarm(object, time::DateTime());
	}

	template<class T>
	    AlarmRecipientTemplate<T>::AlarmRecipientTemplate(const std::string& title)
	    : AlarmRecipient(title)
	{

	}

	template<class T>
	    std::set<const SentAlarm*> AlarmRecipientTemplate<T>::getLinkedAlarms(const T* object)
	{
	    typename LinksSetObject::iterator it = _linksObject.find(object);
	    return (it == _linksObject.end()) ? std::set<const SentAlarm*>() : it->second;
	}

	template<class T>
	    std::set<const T*> AlarmRecipientTemplate<T>::getLinkedObjects(const SentAlarm* alarm)
	{
	    typename  LinksSetAlarm::iterator it = _linksAlarm.find(alarm);
	    return (it == _linksAlarm.end()) ? std::set<const T*>() : it->second;
	}

	template<class T>
	    void AlarmRecipientTemplate<T>::remove(const T* object, const SentAlarm* alarm)
	{
	    typename LinksSetObject::iterator it = 
		_linksObject.find(object);

	    if (it != _linksObject.end())
	    {
			std::set<const SentAlarm*>::iterator its = it->second.find(alarm);
			if (its != it->second.end())
				it->second.erase(its);
	    }

	    typename  LinksSetAlarm::iterator it2 = AlarmRecipientTemplate<T>::_linksAlarm.find(alarm);
	    if (it2 != AlarmRecipientTemplate<T>::_linksAlarm.end())
	    {
			typename std::set<const T*>::iterator its = it2->second.find(object);
			if (its != it2->second.end())
				it2->second.erase(its);
	    }
	}

	template<class T>
	    void AlarmRecipientTemplate<T>::add(const T* object, const SentAlarm* alarm)
	{
	    typename LinksSetObject::iterator it = _linksObject.find(object);
	    if (it == _linksObject.end())
	    {
			std::set<const SentAlarm*> s;
			s.insert(alarm);
			_linksObject.insert(make_pair(object, s));
	    }
	    else
		it->second.insert(alarm);

	    typename  LinksSetAlarm::iterator it2 = _linksAlarm.find(alarm);
	    if (it2 == _linksAlarm.end())
	    {
			std::set<const T*> s;
			s.insert(object);
			_linksAlarm.insert(make_pair(alarm, s));
	    }
	    else
		it2->second.insert(object);
	}
    }
}

#endif // SYNTHESE_AlarmRecipientTemplate_h__
