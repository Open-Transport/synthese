


function insert_select(select, value, text){
    var optionElement = document.createElement("option");
    optionElement.value = value;
    optionElement.text = text;
    var i;
    for(i=0; i<select.options.length; ++i) {
      if(select.options[i].text > text) {
        select.add(optionElement, select.options[i]);
        break;
      }
    }
    if(i==select.options.length){
      select.add(optionElement, null);
    }
}

function get_add_row(tableId)
{
  return document.getElementById(tableId).tFoot.rows[0];
}

function remove_row(link)
{
  var row = link.parentNode.parentNode;
  var tableId = row.parentNode.parentNode.id;

  var confirmation = removeConfirmation[tableId] ? removeConfirmation[tableId](row) : "Are you sure to remove the row ?";
  if(confirm(confirmation))
  {
    var addRow=get_add_row(tableId);
    for(var i=0; i<addRow.cells.length;++i) {
      if(addRow.cells[i].childNodes[0].tagName == 'SELECT' &&
         addRow.cells[i].childNodes[0].className == 'unique') {
        insert_select(addRow.cells[i].childNodes[0], row.cells[i].childNodes[0].value, row.cells[i].childNodes[1].data);
        addRow.style.display = '';
    } }
    var tblBody = row.parentNode;
    var j=row.sectionRowIndex;

    if(tblBody.className!='autoorder') {
      if(j == 0 && j < tblBody.rows.length-1) {
        tblBody.rows[j+1].cells[tblBody.rows[j+1].cells.length-1].childNodes[2].innerHTML = '';
      }
      if(j == tblBody.rows.length-1 && tblBody.rows.length>1) {
        tblBody.rows[j-1].cells[tblBody.rows[j-1].cells.length-1].childNodes[1].innerHTML = '';
      }
    }

    tblBody.deleteRow(j);
    for(;j<tblBody.rows.length;++j) {
      tblBody.rows[j].className = 'r'+ (j % 2 + 1);
    }
  }
}

function add_new_row(link)
{
  var tableId = link.parentNode.parentNode.parentNode.parentNode.id;
  var addRow = get_add_row(tableId);
  var values = new Array();
  for(var i=0; i<addRow.cells.length; ++i) {
    if(addRow.cells[i].childNodes[0].tagName == 'SELECT') {
      var select = addRow.cells[i].childNodes[0];
      values[i] = select.options[select.selectedIndex].value;
    } else if(addRow.cells[i].childNodes[0].tagName == 'INPUT') {
      values[i] = addRow.cells[i].childNodes[0].value;
      addRow.cells[i].childNodes[0].value = '';
    } else {
      values[i] = '';
    }
  }
  add_row(tableId, values);
}

function add_row(tableId, values)
{
  var addRow = get_add_row(tableId);
  var tblBody = document.getElementById(tableId).tBodies[0];

  var j;
  if(tblBody.className=="autoorder") {
    var key = '';
    for(var i=0; i<addRow.cells.length; ++i) {
      if(addRow.cells[i].childNodes[0].tagName == 'SELECT') {
        var select = addRow.cells[i].childNodes[0];
        for(k=0; k<select.options.length; ++k) {
          if(select.options[k].value == values[i]) {
            break;
        } }
        key += select.options[k].text +'|';
      } else if(addRow.cells[i].childNodes[0].tagName == 'INPUT') {
        key += values[i] +'|';
      }
    }
    for(var j=0; j<tblBody.rows.length; ++j) {
      var key2 = '';
      for(var i=0; i<addRow.cells.length; ++i) {
        if(addRow.cells[i].childNodes[0].tagName == 'SELECT') {
          key2 += tblBody.rows[j].cells[i].childNodes[1].data +'|';
        } else if(addRow.cells[i].childNodes[0].tagName == 'INPUT') {
          key2 += tblBody.rows[j].cells[i].childNodes[0].value +'|';
        }
      }
      if(key2 > key) {
        break;
    } }
  } else {
    j = tblBody.rows.length;
  }
  var newRow = tblBody.insertRow(j);

  for(var i=0; i<addRow.cells.length; ++i) {
    var newCell = newRow.insertCell(i);
    if(addRow.cells[i].childNodes[0].tagName == 'SELECT') {
      var select = addRow.cells[i].childNodes[0];
      for(k=0; k<select.options.length; ++k) {
        if(select.options[k].value == values[i]) {
          break;
      } }
      newCell.innerHTML = '<input type="hidden" value="'+ values[i] +'" />' + select.options[k].text;
      if(select.className == 'unique') {
        select.remove(k);
      }
      if(select.options.length == 0) {
        addRow.style.display = 'none';
      }
    } else if(addRow.cells[i].childNodes[0].tagName == 'INPUT') {
      newCell.innerHTML = '<input type="text" value="'+ values[i] +'"  />';
    } else if(i==addRow.cells.length-1) {
      var html = '<a href="#" onclick="remove_row(this); return false;"><img src="delete.png" alt="-" title="Supprimer l\'élément" /></a>';
      if(tblBody.className!='autoorder') {
        html += '<span class="up_down_button"></span><span class="up_down_button">';
        if(tblBody.rows.length > 1) {
          html += '<a href="#" onclick="up_row(this); return false;"><img src="arrow_up.png" alt="-" title="Monter l\'élément" /></a>';
          tblBody.rows[j-1].cells[tblBody.rows[j-1].cells.length-1].childNodes[1].innerHTML = '<a href="#" onclick="down_row(this); return false;"><img src="arrow_down.png" alt="-" title="Descendre l\'élément" /></a>';
        }
        html += '</span>';
      }
      newCell.innerHTML = html;
    } else {
      newCell.innerHTML = addRow.cells[i].innerHTML;
  } }

  for(j;j<tblBody.rows.length;++j) {
    tblBody.rows[j].className = 'r'+ (j % 2 + 1);
  }
}

function key_press(event) {
  if (!event) var event = window.event;
  if(event.keyCode==13){
    var elem = event.target;
    if(!elem) elem = event.srcElement;
    add_new_row(elem.parentNode.parentNode.cells[elem.parentNode.parentNode.cells.length-1].childNodes[0]);
  }
}

function send_vector_field(tableId)
{
  var addRow = get_add_row(tableId);
  var tblBody = document.getElementById(tableId).tBodies[0];
  var content = '';
  for(var i=0; i<tblBody.rows.length; ++i)
  {
    var row = tblBody.rows[i];
    var value='';
    for(var j=0; j<addRow.cells.length; ++j) {
      if(addRow.cells[j].childNodes[0].tagName == 'SELECT' ||
         addRow.cells[j].childNodes[0].tagName == 'INPUT') {
        if(value.length>0) value += '|';
        value += row.cells[j].childNodes[0].value;
    } }
    if(content.length) content += ',';
    content += value;
  }
  return content;
}

function down_row(link)
{
  var row = link.parentNode.parentNode.parentNode;
  var nextRow = row.nextSibling;
  for(var i=0; i<row.cells.length-1; ++i) {
    var content = row.cells[i].innerHTML;
    row.cells[i].innerHTML = nextRow.cells[i].innerHTML;
    nextRow.cells[i].innerHTML = content;
  }
}

function up_row(link)
{
  var row = link.parentNode.parentNode.parentNode;
  var prevRow = row.previousSibling;
  for(var i=0; i<row.cells.length-1; ++i) {
    var content = row.cells[i].innerHTML;
    row.cells[i].innerHTML = prevRow.cells[i].innerHTML;
    prevRow.cells[i].innerHTML = content;
  }
}

var removeConfirmation = new Array();