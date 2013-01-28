
/** TinyMCE class implementation.
	@file TinyMCE.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "TinyMCE.hpp"
#include "HTMLModule.h"

#include <sstream>

using namespace std;

namespace synthese
{
	namespace html
	{
		const string TinyMCE::MCE_EDITOR_ACTIVATION_CLASS("mceEditor");



		TinyMCE::TinyMCE():
			_withImageManager(false),
			_withFileManager(false),
			_withPageBreak(false),
			_withSpellChecker(false),
			_withEmoticons(false)
		{}



		string TinyMCE::open() const
		{
			stringstream s;
			s << HTMLModule::GetHTMLJavascriptOpen();

			s <<
				"tinyMCE.init({" <<
				"mode : \"specific_textareas\"" <<
				",editor_selector : \"" << MCE_EDITOR_ACTIVATION_CLASS << "\"" <<
				",theme : \"advanced\"" <<
				",entites:''" <<
				",entity_encoding:'named'" <<
				",setup : function(ed) {" <<
					//This event (onBeforeSetContent)gets executed when the setContent method is called
					//gets executed when the setContent method is called but before the contents gets
					//serialized and placed in the editor.
					"ed.onBeforeSetContent.add(function(ed, o) {" <<
						//replace all instances of <? and ?> with HTML entities
						"o.content = o.content.replace(/<\\?/gi, \"&lt;?\");" <<
						"o.content = o.content.replace(/\\?>/gi, \"?&gt;\");" <<
					"});" <<
				"}" <<
				",plugins : \"safari,";
			if(_withSpellChecker)
			{
				s << ",spellchecker";
			}
			if(_withPageBreak)
			{
				s << ",pagebreak";
			}
			if(_withImageManager)
			{
				s << ",imagemanager";
			}
			if(_withFileManager)
			{
				s << ",filemanager";
			}
			if(_withEmoticons)
			{
				s << ",emotions";
			}
			if(!_ajaxSaveURL.empty())
			{
				s << ",save";
			}

			s <<
				",style,layer,table,advhr,advimage,advlink,iespell,inlinepopups,insertdatetime,preview,media,searchreplace,print,contextmenu,paste,directionality,fullscreen,noneditable,visualchars,nonbreaking,xhtmlxtras,template\"" <<
				",theme_advanced_buttons1 : \"";
			if(!_ajaxSaveURL.empty())
			{
				s << "save,";
			}
			s << "newdocument,|,bold,italic,underline,strikethrough,|,justifyleft,justifycenter,justifyright,justifyfull,|,styleselect,formatselect,fontselect,fontsizeselect\"" <<
				",theme_advanced_buttons2 : \"cut,copy,paste,pastetext,pasteword,|,search,replace,|,bullist,numlist,|,outdent,indent,blockquote,|,undo,redo,|,link,unlink,anchor,image,cleanup,help,code,|,insertdate,inserttime,preview,|,forecolor,backcolor\"" <<
				",theme_advanced_buttons3 : \"tablecontrols,|,hr,removeformat,visualaid,|,sub,sup,|,charmap";
			if(_withEmoticons)
			{
				s << ",emotions";
			}
			s << ",iespell,media,advhr,|,print,|,ltr,rtl,|,fullscreen\"" <<
				",theme_advanced_buttons4 : \"insertlayer,moveforward,movebackward,absolute,|,styleprops";
			if(_withSpellChecker)
			{
				s << ",spellchecker";
			}
			s << ",|,cite,abbr,acronym,del,ins,attribs,|,visualchars,nonbreaking,template,blockquote";
			if(_withPageBreak)
			{
				s << ",pagebreak";
			}
			if(_withFileManager || _withImageManager)
			{
				s << ",|";
				if(_withFileManager)
				{
					s << ",insertfile";
				}
				if(_withImageManager)
				{
					s << ",insertimage";
				}
			}

			s << "\"" <<
				",theme_advanced_toolbar_location : \"top\"" <<
				",theme_advanced_toolbar_align : \"left\"" <<
				",theme_advanced_statusbar_location : \"bottom\"" <<
				",theme_advanced_resizing : true";
			if(!_ajaxSaveURL.empty())
			{
				s << ",save_onsavecallback : \"save_ajax\"";
			}
			s << "});";

			if(!_ajaxSaveURL.empty())
			{
				s <<
					"function save_ajax(ed)" <<
					"{" <<
						"ed.setProgressState(1);" << // Show progress

						"var xhr;" <<
						"try {  xhr = new ActiveXObject('Msxml2.XMLHTTP');   }" <<
						"catch (e) " <<
						"{" <<
							"try {   xhr = new ActiveXObject('Microsoft.XMLHTTP');    }" <<
							"catch (e2) " <<
							"{" <<
								"try {  xhr = new XMLHttpRequest();     }" <<
								"catch (e3) {  xhr = false;   }" <<
							"}" <<
						"}" <<

						"xhr.onreadystatechange  = function()" <<
						"{ " <<
							"if(xhr.readyState  == 4)" <<
							"{" <<
								"if(xhr.status != 200) " <<
									"alert('Error during save action');" <<
								"ed.setProgressState(0);" << // Hide progress
							"}" <<
						"}; " <<

						"xhr.open('POST','" << _ajaxSaveURL << "');" <<
						"xhr.setRequestHeader('Content-Type','application/x-www-form-urlencoded');" <<
						"xhr.send(ed.getElement().name +'='+ encodeURIComponent(ed.getContent()));" <<
					"}"
				;
			}

			s << HTMLModule::GetHTMLJavascriptClose();

			return s.str();
		}



		std::string TinyMCE::GetFakeFormWithInput(
			const std::string& name,
			const std::string& value
		){
			stringstream s;
			s <<
				"<form action=\"\" onsubmit=\"return false;\">" <<
				"<textarea name=\"" << name << "\" class=\"" << MCE_EDITOR_ACTIVATION_CLASS << "\">" <<
				value <<
				"</textarea></form>"
			;
			return s.str();
		}
	}
}
