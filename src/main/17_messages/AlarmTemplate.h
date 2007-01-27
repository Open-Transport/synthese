namespace synthese
{
	namespace messages
	{
		class AlarmTemplate : public util::Registrable<AlarmTemplate, uid>
		{
		private:
			bool				_allTypes;
			Alarm::AlarmLevel	_type;
			std::string			_shortMessage;
			std::string			_longMessage;

		public:
			AlarmTemplate();
			~AlarmTemplate();

		};
	}
}