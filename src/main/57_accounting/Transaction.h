namespace synthese
{
	namespace accounting
	{
		class Transaction
		{
//			Document*	_document;
			time::DateTime	_dateTime;
			User*			_leftUser;	// Applicable if the used accounts are template
			Place*			_place;
			uid				_id;
		}
	}
}