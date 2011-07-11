////////////////////////////////////////////////////////////////////////////////
/// AlarmRecipientTemplate class header.
///	@file AlarmRecipientTemplate.h
///	@author Hugues Romain
///	@date 2008-12-26 21:19
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_AlarmRecipientTemplate_h__
#define SYNTHESE_AlarmRecipientTemplate_h__

#include <set>
#include <map>

#include "AlarmRecipient.h"
#include "SentAlarm.h"
#include "FactorableTemplate.h"
#include "SentScenario.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
    namespace messages
    {
	/** AlarmRecipientTemplate class.
	    @ingroup m17
	*/
	template<class T, class C>
	class AlarmRecipientTemplate : public util::FactorableTemplate<AlarmRecipient, C>
	{
	public:
		typedef typename T::ObjectType									ObjectType;
		typedef std::set<const ObjectType*>								LinkedObjectsSet;
		typedef std::set<const SentAlarm*>								LinkedAlarmsSet;
	    typedef typename std::map<const SentAlarm*, LinkedObjectsSet>	AlarmLinks;
		typedef typename std::map<const ObjectType*, LinkedAlarmsSet>	ObjectLinks;

	private:
	    static AlarmLinks	_linksAlarm;
	    static ObjectLinks	_linksObject;

	protected:
	    static void add(const ObjectType* object, const SentAlarm* alarm);
	    static void remove(const ObjectType* object, const SentAlarm* alarm);

		virtual const std::string& getTitle() const;

		virtual const util::RegistryTableType getTableId() const;

	public:
		static const std::string TITLE;

	    static LinkedAlarmsSet	getLinkedAlarms(const ObjectType* object);
	    static LinkedObjectsSet	getLinkedObjects(const SentAlarm* alarm);

	    static const SentAlarm* getAlarm(const ObjectType* object);
		static const SentAlarm* getAlarm(const ObjectType* object, const boost::posix_time::ptime& date);

		static void getStaticParametersLabels(security::ParameterLabelsVector& m);

		static util::RegistryKeyType GetObjectIdBySource(
			const impex::DataSource& source,
			const std::string& key,
			util::Env& env
		);

		/** Catch of an alarm conflict for a specified recipient.
			@param object The recipient to analyze
			@return synthese::messages::AlarmConflict the "worse" conflict status of all the alarms to display on the recipient.
			@author Hugues Romain
			@date 2007
		*/
		static AlarmConflict getConflicStatus(const T* object);

		/** Catch of a conflict for a specified alarm regarding the current recipient type only.
			@param alarm The alarm to analyze
			@return synthese::messages::AlarmConflict the "worse" conflict between the alarm and all the others alarm to be displayed on the recipients ones
			@author Hugues Romain
			@date 2007
			To obtain the global alarm conflict status, use the public method Alarm::getConflictStatus
		*/
		AlarmConflict getConflictStatus(const SentAlarm* alarm) const;

		virtual util::RegistryKeyType getObjectIdBySource(
			const impex::DataSource& source,
			const std::string& key,
			util::Env& env
		) const;

		virtual void getStaticParametersLabelsVirtual(security::ParameterLabelsVector& m);
	};



	template<class T, class C>
	util::RegistryKeyType synthese::messages::AlarmRecipientTemplate<T, C>::getObjectIdBySource(
		const impex::DataSource& source,
		const std::string& key,
		util::Env& env
	) const {
		return GetObjectIdBySource(source, key, env);
	}

	template<class T, class C>
	const util::RegistryTableType synthese::messages::AlarmRecipientTemplate<T, C>::getTableId() const
	{
		return T::TABLE.ID;
	}

	template<class T, class C>
	void synthese::messages::AlarmRecipientTemplate<T, C>::getStaticParametersLabelsVirtual( security::ParameterLabelsVector& m )
	{
		getStaticParametersLabels(m);
	}

	template<class T, class C>
	const std::string& synthese::messages::AlarmRecipientTemplate<T, C>::getTitle() const
	{
		return TITLE;
	}

	template<class T, class C>
	AlarmConflict synthese::messages::AlarmRecipientTemplate<T, C>::getConflictStatus( const SentAlarm* alarm ) const
	{
		typename AlarmLinks::const_iterator it = _linksAlarm.find(alarm);
		if (it == _linksAlarm.end())
			return ALARM_CONFLICT_UNKNOWN;

		AlarmConflict conflictStatus(ALARM_NO_CONFLICT);
		for (typename LinkedObjectsSet::const_iterator ita = it->second.begin(); ita != it->second.end(); ++ita)
		{
			typename ObjectLinks::const_iterator ito = _linksObject.find(*ita);
			if (ito == _linksObject.end())
				return ALARM_CONFLICT_UNKNOWN; /// @todo throw an exception
			for (LinkedAlarmsSet::const_iterator itb = ito->second.begin(); itb != ito->second.end(); ++itb)
			{
				if (*itb == alarm)
					continue;

				AlarmConflict thisConflictStatus = alarm->wereInConflictWith(**itb);
				if (thisConflictStatus > conflictStatus)
					conflictStatus = thisConflictStatus;
				if (conflictStatus == ALARM_CONFLICT)
					return conflictStatus;
			}
		}
		return conflictStatus;
	}

	template<class T, class C>
		AlarmConflict AlarmRecipientTemplate<T, C>::getConflicStatus( const T* object )
	{
		typename ObjectLinks::const_iterator it = _linksObject.find(object);
		if (it == _linksObject.end())
			return ALARM_CONFLICT_UNKNOWN;

		AlarmConflict conflictStatus(ALARM_NO_CONFLICT);
		for (LinkedAlarmsSet::const_iterator ita = it->second.begin(); ita != it->second.end(); ++ita)
			for (LinkedAlarmsSet::const_iterator itb = it->second.begin(); itb != it->second.end(); ++itb)
			{
				if (ita == itb)
					continue;

				AlarmConflict thisConflictStatus = (*ita)->wereInConflictWith(**itb);
				if (thisConflictStatus > conflictStatus)
					conflictStatus = thisConflictStatus;
				if (conflictStatus == ALARM_CONFLICT)
					return conflictStatus;
			}
		return conflictStatus;
	}

	template<class T, class C>
	const SentAlarm* AlarmRecipientTemplate<T, C>::getAlarm( const ObjectType* object, const boost::posix_time::ptime& date )
	{
	    typename ObjectLinks::const_iterator it = _linksObject.find(object);
	    if (it == _linksObject.end())
			return NULL;

	    const SentAlarm* alarm = NULL;
	    for (LinkedAlarmsSet::const_iterator its = it->second.begin(); its != it->second.end(); ++its)
	    {
			const SentAlarm* candidateAlarm = *its;

			if(	candidateAlarm->getScenario() &&
				candidateAlarm->getScenario()->isApplicable(date) && (
					alarm == NULL ||
					candidateAlarm->getLevel() > alarm->getLevel() ||
					candidateAlarm->getLevel() == alarm->getLevel() &&
					!candidateAlarm->getScenario()->getPeriodStart().is_not_a_date_time() &&
					candidateAlarm->getScenario()->getPeriodStart() > alarm->getScenario()->getPeriodStart()
			)	) alarm = candidateAlarm;

	    }
	    return alarm;
	}

	template<class T, class C>
	    const SentAlarm* AlarmRecipientTemplate<T, C>::getAlarm( const ObjectType* object )
	{
		boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
	    return AlarmRecipientTemplate<T, C>::getAlarm(object, now);
	}

	template<class T, class C>
	    typename AlarmRecipientTemplate<T, C>::LinkedAlarmsSet AlarmRecipientTemplate<T,C>::getLinkedAlarms(const ObjectType* object)
	{
	    typename ObjectLinks::const_iterator it = _linksObject.find(object);
	    return (it == _linksObject.end()) ? LinkedAlarmsSet() : it->second;
	}

	template<class T, class C>
	    typename AlarmRecipientTemplate<T, C>::LinkedObjectsSet AlarmRecipientTemplate<T,C>::getLinkedObjects(const SentAlarm* alarm)
	{
	    typename  AlarmLinks::const_iterator it = _linksAlarm.find(alarm);
	    return (it == _linksAlarm.end()) ? LinkedObjectsSet() : it->second;
	}

	template<class T, class C>
	    void AlarmRecipientTemplate<T, C>::remove(const ObjectType* object, const SentAlarm* alarm)
	{
	    typename ObjectLinks::iterator it =
		_linksObject.find(object);

	    if (it != _linksObject.end())
	    {
			LinkedAlarmsSet::iterator its = it->second.find(alarm);
			if (its != it->second.end())
				it->second.erase(its);
	    }

	    typename  AlarmLinks::iterator it2 = AlarmRecipientTemplate<T,C>::_linksAlarm.find(alarm);
	    if (it2 != AlarmRecipientTemplate<T,C>::_linksAlarm.end())
	    {
			typename LinkedObjectsSet::iterator its = it2->second.find(object);
			if (its != it2->second.end())
				it2->second.erase(its);
	    }
	}

	template<class T, class C>
	    void AlarmRecipientTemplate<T, C>::add(const ObjectType* object, const SentAlarm* alarm)
	{
	    typename ObjectLinks::iterator it = _linksObject.find(object);
	    if (it == _linksObject.end())
	    {
			LinkedAlarmsSet s;
			s.insert(alarm);
			_linksObject.insert(make_pair(object, s));
	    }
	    else
		it->second.insert(alarm);

	    typename  AlarmLinks::iterator it2 = _linksAlarm.find(alarm);
	    if (it2 == _linksAlarm.end())
	    {
			LinkedObjectsSet s;
			s.insert(object);
			_linksAlarm.insert(make_pair(alarm, s));
	    }
	    else
		it2->second.insert(object);
	}
    }
}

#endif // SYNTHESE_AlarmRecipientTemplate_h__
