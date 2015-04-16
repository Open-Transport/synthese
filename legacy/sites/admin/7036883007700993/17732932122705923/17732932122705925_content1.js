function addTextInputAutoComplete(params) {
  if (params.fieldId.length == 0)
    params.fieldId = params.name + "__ID";
  
  var fieldIdAutoComplete;
  if (params.useId)
    fieldIdAutoComplete = params.fieldId + "Selection__ID";
  else
    fieldIdAutoComplete = params.fieldId;

  var content;

  // Add divs
  content = "<div id=\"div" + params.fieldId + "Selection\" style=\"min-width:170px\">";
  content = content + "<input type=\"text\" name=\"" + (params.useId ? params.name + "Selection": params.name) + "\" value=\"" + params.valueName + "\" id=\"" + fieldIdAutoComplete + "\" style=\"width:144px; margin-right:0px;\"/>";
  if (params.bottomButton) {
    content = content + "<input type=\"button\" value=\"v\" onclick=\"if ($('.ui-autocomplete').is(':visible')) $('#" + fieldIdAutoComplete + "').autocomplete('close','');else {$('#" + fieldIdAutoComplete + "').autocomplete('search','');$('#" + fieldIdAutoComplete + "').focus();}\" style=\"margin-left:0px;margin-right:0px;\"/> ";
  }
  if (params.idButton) {
    content = content + "<input type=\"button\" value=\"ID\" onclick=\"$('#div" + params.fieldId + "Selection').hide();$('#div" + params.fieldId + "').show();\" style=\"margin-left:0px;\"/>";
  }
  content = content + "</div>";
  if (params.useId) {
    content = content + "<div id=\"div" + params.fieldId + "\" style=\"display:none\">";
    content = content + "<input type=\"text\" name=\"" + params.name + "\" value=\"" + params.valueId + "\" id=\"" + params.fieldId + "\" style=\"margin-right:0px;\" />";
    content = content + "<input type=\"button\" value=\"ID\" onclick=\"$('#div" + params.fieldId + "Selection').show();$('#div" + params.fieldId + "').hide();\" style=\"margin-left:0px;\" />";
    content = content + "</div>";
  }

  // Replace input with 
  $('input[type="text"][id="' + params.fieldId + '"]').replaceWith(content);

  // Execute js code
  var jscode = "$(function() {"
    + "$('#" + fieldIdAutoComplete + "').autocomplete({"
      + "source: function( request, response ) {"
        + "$.ajax({"
          + "url: 'synthese',"
          + "dataType: 'json',"
          + "mode: 'abort',"
          + "port: 'autocomplete"+ params.fieldId + "',"
          + "data: {"
            + "SERVICE: '" + params.service + "',"
            + "output_format: 'json',"
            + "n: 12,"
            + "t: request.term";
            if (params.extraParamName.length > 0) {
              jscode = jscode + ", " + params.extraParamName + ": ";
              if (params.extraParamInputName.length > 0) jscode = jscode + "$('input[type=text][name="+params.extraParamInputName+"]').val()";
              else jscode = jscode + "''";
            }
            if (params.service == "lr")
              jscode = jscode + ", table: "  + params.tableId;

          jscode = jscode + "},"
          + "success: function( data ) {"
            + "if ($('#" + fieldIdAutoComplete + "').is(':focus'))"
              + "response( $.map( data." + params.rows + "." + params.row + ", function( item ) {"
                + "return {"
                  + "label: item.name" + (params.viewId ? " + ' (' + item.roid + ')'," : ",")
                  + "value: item.name,"
                  + "id: item.roid"
                + "}"
            + "}));"
          + "}"
        + "});"
      + "},";
      if (params.useId) {
        jscode = jscode + "select: function( event, ui ) {"
          + "$('#" + params.fieldId + "').val(ui.item.id);"
        + "},";
      }
      if (params.bottomButton) {
        jscode = jscode + "minLength: 0,";
      } else {
        jscode = jscode + "minLength: 3,";
      }
      jscode = jscode + "autoFocus: false,"
      + "delay: 150,"
      + "open: function() {"
        + "$(this).removeClass('ui-corner-all').addClass('ui-corner-top');"
      + "},"
      + "blur : function() {"
        + "$(this).removeClass('ui-corner-top').addClass('ui-corner-all');"
      + "},"
      + "close: function() {"
        + "$(this).removeClass('ui-corner-top').addClass('ui-corner-all');"
      + "}"
    + "});"
  + "});";

  eval(jscode);
}


/**
 * Function: setGeoRM
 * Assign Geoportal's GeoRM token to an Object.
 *
 * Returns:
 * {Object} the rightsManagement key
 */
function setGeoRM() {
    return Geoportal.GeoRMHandler.addKey(
        gGEOPORTALRIGHTSMANAGEMENT.apiKey,
        gGEOPORTALRIGHTSMANAGEMENT[gGEOPORTALRIGHTSMANAGEMENT.apiKey[0]].tokenServer.url,
        gGEOPORTALRIGHTSMANAGEMENT[gGEOPORTALRIGHTSMANAGEMENT.apiKey[0]].tokenServer.ttl,
        map);
}
/*
 * Copyright (c) 2008-2012 Institut National de l'information Geographique et forestiere France, released under the
 * BSD license.
 */

if (window.__Geoportal$timer===undefined) {
    var __Geoportal$timer= null;
}

/**
 * Function: checkApiLoading
 * Assess that needed classes have been loaded.
 *
 * Parameters:
 * retryClbk - {Function} function to call if any of the expected classes
 * is missing.
 * clss - {Array({String})} list of classes to check.
 *
 * Returns:
 * {Boolean} true when all needed classes have been loaded, false otherwise.
 */
function checkApiLoading(retryClbk,clss) {
    if (__Geoportal$timer!=null) {
        //clearTimeout: cancels the timer "__Geoportal$timer" before its end
        //clearTimeout: annule le minuteur "__Geoportal$timer" avant sa fin
        window.clearTimeout(__Geoportal$timer);
         __Geoportal$timer= null;
    }

    /**
    * It may happen that the init function is executed before the API is loaded
    * Addition of a timer code that waits 300 ms before running the init function
    *
    * Il se peut que l'init soit exÃ©cutÃ© avant que l'API ne soit chargÃ©e
    * Ajout d'un code temporisateur qui attend 300 ms avant de relancer l'init
    */
    var f;
    for (var i=0, l= clss.length; i<l; i++) {
        try {
            f= eval(clss[i]);
        } catch (e) {
            f= undefined;
        }
        if (typeof(f)==='undefined') {
             __Geoportal$timer= window.setTimeout(retryClbk, 300);
            return false;
        }
    }
    return true;
}

function ajaxMapChange(obj) {
  var url=obj.action+'?';
  var first=1;
  for (var i = 0; i < obj.elements.length; ++i) {
    if ((obj.elements[i].type == 'radio') && (!obj.elements[i].checked)) continue;
    if (first) {
      first = 0;
    } else {
      url += '&';
    }
    url += obj.elements[i].name +'='+ encodeURIComponent(obj.elements[i].value);
  }

  var xajax = null;
  if (window.XMLHttpRequest) xajax = new XMLHttpRequest();
  else if (window.ActiveXObject) xajax = new ActiveXObject('Microsoft.XMLHTTP');
  else return false;
  xajax.open('GET',url);
  xajax.onreadystatechange = function() {
    if (xajax.readyState == 4) {
      var oldProjection = map.getProjectionObject();
      var oldExtent = map.getExtent();
      map.destroy();
      eval(xajax.responseText);
      var newProjection = map.getProjectionObject();
      map.addLayer(getFeaturesLayer(newProjection));
      var newbounds = oldExtent.transform(oldProjection, newProjection);
      map.zoomToExtent(newbounds, true);
    }
  };
  xajax.send();
}


function insert_select(select, value, text) {
  var optionElement = document.createElement("option");
  optionElement.value = value;
  optionElement.text = text;
  var i;
  for (i = 0; i < select.options.length; ++i) {
    if (select.options[i].text > text) {
      select.add(optionElement, select.options[i]);
      break;
    }
  }
  if (i == select.options.length) {
    select.add(optionElement, null);
  }
}

function get_add_row(tableId) {
  return document.getElementById(tableId).tFoot.rows[0];
}

function remove_row(link) {
  var row = link.parentNode.parentNode;
  var tableId = row.parentNode.parentNode.id;

  var confirmation = removeConfirmation[tableId] ? removeConfirmation[tableId](row) : "Are you sure to remove the row ?";
  if (confirm(confirmation))
  {
    var addRow=get_add_row(tableId);
    for (var i = 0; i < addRow.cells.length;++i) {
      if (addRow.cells[i].childNodes[0].tagName == 'SELECT' &&
         addRow.cells[i].childNodes[0].className == 'unique') {
        insert_select(addRow.cells[i].childNodes[0], row.cells[i].childNodes[0].value, row.cells[i].childNodes[1].data);
        addRow.style.display = '';
      }
    }
    var tblBody = row.parentNode;
    var j=row.sectionRowIndex;

    if (tblBody.className!='autoorder') {
      if (j == 0 && j < tblBody.rows.length-1) {
        tblBody.rows[j+1].cells[tblBody.rows[j+1].cells.length-1].childNodes[2].innerHTML = '';
      }
      if (j == tblBody.rows.length-1 && tblBody.rows.length>1) {
        tblBody.rows[j-1].cells[tblBody.rows[j-1].cells.length-1].childNodes[1].innerHTML = '';
      }
    }

    tblBody.deleteRow(j);
    for (; j < tblBody.rows.length; ++j) {
      tblBody.rows[j].className = 'r'+ (j % 2 + 1);
    }
  }
}

function add_new_row(link) {
  var tableId = link.parentNode.parentNode.parentNode.parentNode.id;
  var addRow = get_add_row(tableId);
  var values = new Array();
  for (var i = 0; i < addRow.cells.length; ++i) {
    if (addRow.cells[i].childNodes[0].tagName == 'SELECT') {
      var select = addRow.cells[i].childNodes[0];
      values[i] = select.options[select.selectedIndex].value;
    } else if (addRow.cells[i].childNodes[0].tagName == 'INPUT') {
      values[i] = addRow.cells[i].childNodes[0].value;
      addRow.cells[i].childNodes[0].value = '';
    } else if (addRow.cells[i].childNodes[0].tagName == 'DIV') {
      values[i] = new Array();
      values[i][0] = addRow.cells[i].childNodes[1].childNodes[0].value;
      values[i][1] = addRow.cells[i].childNodes[0].childNodes[0].value;
      addRow.cells[i].childNodes[0].childNodes[0].value = '';
    }
    else {
      values[i] = '';
    }
  }
  add_row(tableId, values);
}

function add_row(tableId, values) {
  var addRow = get_add_row(tableId);
  var tblBody = document.getElementById(tableId).tBodies[0];

  var j;
  if (tblBody.className == "autoorder") {
    var key = '';
    for (var i = 0; i < addRow.cells.length; ++i) {
      if (addRow.cells[i].childNodes[0].tagName == 'SELECT') {
        var select = addRow.cells[i].childNodes[0];
        for (k = 0; k < select.options.length; ++k) {
          if (select.options[k].value == values[i]) {
            break;
        } }
        key += select.options[k].text +'|';
      } else if (addRow.cells[i].childNodes[0].tagName == 'INPUT') {
        key += values[i] +'|';
      }
    }
    for (var j = 0; j < tblBody.rows.length; ++j) {
      var key2 = '';
      for (var i = 0; i < addRow.cells.length; ++i) {
        if (addRow.cells[i].childNodes[0].tagName == 'SELECT') {
          key2 += tblBody.rows[j].cells[i].childNodes[1].data +'|';
        } else if (addRow.cells[i].childNodes[0].tagName == 'INPUT') {
          key2 += tblBody.rows[j].cells[i].childNodes[0].value +'|';
        }
      }
      if (key2 > key) {
        break;
      }
    }
  } else {
    j = tblBody.rows.length;
  }
  var newRow = tblBody.insertRow(j);

  for (var i = 0; i < addRow.cells.length; ++i) {
    var newCell = newRow.insertCell(i);
    if (addRow.cells[i].childNodes[0].tagName == 'SELECT') {
      var select = addRow.cells[i].childNodes[0];
      for (k = 0; k < select.options.length; ++k) {
        if (select.options[k].value == values[i]) {
          break;
      } }
      newCell.innerHTML = '<input type="hidden" value="'+ values[i] +'" />' + select.options[k].text;
      if (select.className == 'unique') {
        select.remove(k);
      }
      if (select.options.length == 0) {
        addRow.style.display = 'none';
      }
    } else if (addRow.cells[i].childNodes[0].tagName == 'INPUT') {
      newCell.innerHTML = '<input type="text" value="'+ values[i] +'"  />';
    } else if (addRow.cells[i].childNodes[0].tagName == 'DIV') {
      newCell.innerHTML = '<input type="hidden" value="'+ values[i][0] +'"  />';
      newCell.innerHTML += values[i][1];
    } else if (i == addRow.cells.length-1) {
      var html = '<a href="#" onclick="remove_row(this); return false;"><img src="/admin/img/delete.png" alt="-" title="Supprimer l\'Ã©lÃ©ment" /></a>';
      if (tblBody.className != 'autoorder') {
        html += '<span class="up_down_button"></span><span class="up_down_button">';
        if (tblBody.rows.length > 1) {
          html += '<a href="#" onclick="up_row(this); return false;"><img src="/admin/img/arrow_up.png" alt="-" title="Monter l\'Ã©lÃ©ment" /></a>';
          tblBody.rows[j-1].cells[tblBody.rows[j-1].cells.length-1].childNodes[1].innerHTML = '<a href="#" onclick="down_row(this); return false;"><img src="/admin/img/arrow_down.png" alt="-" title="Descendre l\'Ã©lÃ©ment" /></a>';
        }
        html += '</span>';
      }
      newCell.innerHTML = html;
    } else {
      newCell.innerHTML = addRow.cells[i].innerHTML;
    }
  }

  for (j; j < tblBody.rows.length; ++j) {
    tblBody.rows[j].className = 'r'+ (j % 2 + 1);
  }
}

function key_press(event) {
  if (!event) var event = window.event;
  if (event.keyCode == 13) {
    var elem = event.target;
    if (!elem) elem = event.srcElement;
    add_new_row(elem.parentNode.parentNode.cells[elem.parentNode.parentNode.cells.length-1].childNodes[0]);
  }
}

function send_vector_field(tableId) {
  var addRow = get_add_row(tableId);
  var tblBody = document.getElementById(tableId).tBodies[0];
  var content = '';
  for (var i = 0; i < tblBody.rows.length; ++i)
  {
    var row = tblBody.rows[i];
    var value='';
    for (var j = 0; j<addRow.cells.length; ++j) {
      if (addRow.cells[j].childNodes[0].tagName == 'SELECT' ||
         addRow.cells[j].childNodes[0].tagName == 'INPUT' ||
         addRow.cells[j].childNodes[0].tagName == 'DIV') {
        if (value.length > 0) value += '|';
        value += row.cells[j].childNodes[0].value;
      }
    }
    if (content.length) content += ',';
    content += value;
  }
  return content;
}

function down_row(link) {
  var row = link.parentNode.parentNode.parentNode;
  var nextRow = row.nextSibling;
  for (var i = 0; i < row.cells.length-1; ++i) {
    var content = row.cells[i].innerHTML;
    row.cells[i].innerHTML = nextRow.cells[i].innerHTML;
    nextRow.cells[i].innerHTML = content;
  }
}

function up_row(link) {
  var row = link.parentNode.parentNode.parentNode;
  var prevRow = row.previousSibling;
  for (var i = 0; i < row.cells.length-1; ++i) {
    var content = row.cells[i].innerHTML;
    row.cells[i].innerHTML = prevRow.cells[i].innerHTML;
    prevRow.cells[i].innerHTML = content;
  }
}

var removeConfirmation = new Array();
