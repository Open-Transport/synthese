// Gestion du click ouverture&fermeture d'un chevron
function openclose()
{
 var div = $(this).siblings('div');
 activate_node($(this), div.hasClass('hide'));
}
function activate_node(icon_selector, value)
{
 var div = icon_selector.siblings('div');
 if(value)
 {
   div.removeClass('hide');
   icon_selector.addClass('icon-chevron-down');
   icon_selector.removeClass('icon-chevron-right');
 }
 else
 {
   icon_selector.addClass('icon-chevron-right');
   icon_selector.removeClass('icon-chevron-down');
   div.addClass('hide');
 }
}




// Affichage de la popup associee au filtre a editer
function show_recipients_click()
{
  var recipient = $(this).attr('factory');
  show_objects(recipient, filters);
  return false;
}

function compareAttributes(attr1, attr2) {
	if (((attr1 != undefined || attr2 != undefined) && attr1 == attr2) 
	|| (attr1 == undefined && attr2 == undefined)) {
	    return true;
	}
	return false;
}

// Recuperation des cases de filtres cochees et sauvegarde dans l'objet filters
function update_filters_field(recipient, filters, checkbox)
{
  var recipients = [];
  var hiddenrecipients = [];
//  filters[recipient +'_recipient'] = recipients;
//  hiddenfilters[recipient +'_recipient'] = hiddenrecipients;
  
  var cbs = $('input[factory="'+ recipient +'"]');
  cbs.each(function(){
    if($(this).get(0).checked)
    {
      recipients.push({
        id: $(this).attr('link_id'),
        recipient_id: $(this).attr('value'),
        parameter: $(this).attr('parameter')
      });
      // Si present, retrait du filtre dans la liste des filtres caches (enfants)
      var length = hiddenrecipients.length;
      for(var i = 0; i < length; ++i)
      {
        if(compareAttributes(hiddenrecipients[i].id, $(this).attr('link_id'))
        && compareAttributes(hiddenrecipients[i].recipient_id, $(this).attr('value'))
        && compareAttributes(hiddenrecipients[i].parameter, $(this).attr('parameter')))
        {
          // Entree trouvee. Supprimer l'entree et arreter la boucle for
          hiddenrecipients.splice(i,1);
          break;
        }
      }
      // Ajout du parent direct si l'element courant n'est pas deja en haut de l'arbre (i.e. name != 0)
      if($(this).attr('name') != '0')
      {
        //var test  = $(this).parent('div');
        //var test0 = $(this).parent().parent('div');
        //var test1 = $(this).parent().parent().parent('div');
        //var test2 = $(this).parent().parent().parent().parent('div');
        var father = $(this).parent().parent().parent().parent('div').find('input[factory]');
        var stopLoop = false;
        while(stopLoop == false)
        {
          if(father.attr('name') != $(this).attr('name'))
          {
            console.log("Parent :");
            console.log('recipient_id: '+father.attr('value')+' parameter: '+ father.attr('parameter') + ' link_id: '+ father.attr('link_id'));
            // L'ajout ne se fait que si le parent n'est pas deja present dans les filtres et les filtres caches
            var mergeRecipients = $.merge($.merge([],recipients), hiddenrecipients);
            var mergeLength = mergeRecipients.length;
            var fatherFound = false;
            for(var j = 0; j < mergeLength; ++j)
            {
              if(compareAttributes(mergeRecipients[j].id, father.attr('link_id'))
              && compareAttributes(mergeRecipients[j].recipient_id, father.attr('value'))
              && compareAttributes(mergeRecipients[j].parameter, father.attr('parameter')))
              {
                // Entree trouvee. Supprimer l'entree et arreter la boucle for
                fatherFound = true;
                break;
              }
            }
            if(fatherFound == false)
            {
              hiddenrecipients.push({
                id: father.attr('link_id'),
                recipient_id: father.attr('value'),
                parameter: father.attr('parameter')
              });
            }
          }
          if(father.attr('name') == '0')
          {
            stopLoop = true;
          }
          else
          {
            father = father.parent().parent().parent().parent('div').find('input[factory]');
          }
        }
      }
      
      // Ajout des enfants du destinataire checked
      var sons = $(this).parent().parent().children('div').find('input[factory]');
      sons.each(function(){
        var lentgh = hiddenrecipients.length;
        var found = false;
        // Parcours de filtres caches a la recherche du fils que l'on veut ajouter
        for(var i = 0; i < length; ++i)
        {
          if(compareAttributes(hiddenrecipients[i].id, $(this).attr('link_id'))
          && compareAttributes(hiddenrecipients[i].recipient_id, $(this).attr('value'))
          && compareAttributes(hiddenrecipients[i].parameter, $(this).attr('parameter'))
          )
          {
            found = true;
            break;
          }
        }
        // Si le fils n'a pas ete trouve, ajout de celui-ci dans la liste des filtres caches
        if (found == false)
        {
          hiddenrecipients.push({
            id: $(this).attr('link_id'),
            recipient_id: $(this).attr('value'),
            parameter: $(this).attr('parameter')
          });
        }
      });
    }
  });
  
  filters[recipient +'_recipient'] = recipients;
  hiddenfilters[recipient +'_recipient'] = hiddenrecipients;
}

// Complete la liste des filtres en ajoutant les fils des filtres selectionnes
function update_hiddenfilters_field(recipient, filters)
{
  var hiddenrecipients = [];
  for(var i=0; i<filters[recipient+'_recipient'].length; ++i)
  {
    var sons = $('input[factory="'+recipient+'"][value="'+filters[recipient+'_recipient'][i]['recipient_id']+'"]');
    console.log(sons.parentsUntil("div"));
  }
}


// Affichage de la popup des filtres selectionnes (cases cochees) pour un type de destinataire donne (recipient)
function show_objects(recipient, filters)
{
  // Clean
  var cbs = $('input[factory="'+ recipient +'"]');
  cbs.each(function(){
    $(this).get(0).checked = false;
    $(this).removeAttr('link_id');
  });
  
  var links = filters[recipient +'_recipient'];
  for(var i=0; i<links.length; ++i)
  {
    var link = links[i];
    if(link.parameter)
    {
      var input = $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][parameter="'+ link.parameter +'"]');
      input.get(0).checked=true;
      input.attr('link_id', link.id);
      input.parentsUntil('.modal_body').each(function(){activate_node($(this).children('.openclose'),true);});
    }
    else
    {
      var input = $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][noparam]');
      input.get(0).checked=true;
      input.attr('link_id', link.id);
      input.parentsUntil('.modal_body').each(function(){activate_node($(this).children('.openclose'),true);});
    }
  }
  $('#m_'+ recipient).modal();
}





// Affichage des destinataires d'un evenement
function print_recipients(){
  var id = $(this).attr('data-sid');
  var cat = $(this).attr('data-cat');
  print_recipient_view('line', theevent[id], id, cat);
  print_recipient_view('stoparea', theevent[id], id, cat);
  print_recipient_view('displayscreen', theevent[id], id, cat);
  $('#mRecipientsList-'+id+'-'+cat).modal('show');
  return false;
}

function print_recipient_view(recipient, event, id, cat)
{
  var links = [];
  if(event != undefined)
  {
    if(event.calendar != undefined)
    {
      for(var i=0; i<event.calendar.length; ++i)
      {
        if (event.calendar[i].message != undefined)
        {
          for(var j=0; j<event.calendar[i].message.length; ++j)
          {
            var current_recipient=event.calendar[i].message[j][recipient+'_recipient'];
            // Cas ou le recipient est vide : Tous pour line et stoparea, Aucun pour displayscreen
            if(current_recipient.length == 0 && recipient != 'displayscreen') {
              links.push({'id': "", 'parameter': "", 'recipient_id': "0"});
            }
            else {
              for(var k=0; k<current_recipient.length; ++k)
              {
                links.push(current_recipient[k]);
              }
            }
          } 
        }
      }
    }
  }
  var s='';
  if(links.length)
  {
    for(var i=0; i<links.length; ++i)
    {
      var link = links[i];
      // Si un link correspond a "Tous" (cas des lignes et arrets, recipient_id==0 ne peut etre verifie 
      // dans le cas de displayscreen du fait qu'il n'est pas sauve dans ce cas la), 
      // il prend le dessus sur les autres (qui ne se retrouvent plus dans le resultat)
      if(link.recipient_id == 0)
      {
        s = "Aucun";
        break;
      }
      else
      {
        if(s.length) {
          s += '<br />';
        }
        if(link.parameter)
        {
          s += $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][parameter="'+ link.parameter +'"]').closest('label').text();
        }
        else
        {
          var input = $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][noparam]');
          s += input.closest('label').text();
        }
      }
    }
  }
  else
  {
    s = (recipient == 'displayscreen' ? "Aucun" : "Tous");
  }
  $('#view_recipients_'+ recipient + '-' + id + '-' + cat).html(s);
}

function update_filters_preview(recipient, filters)
{
  var links = filters[recipient +'_recipient'];
  var s='';
  if(links.length)
  {
    for(var i=0; i<links.length; ++i)
    {
      var link = links[i];
      if(s.length)
      {
        s += '<br />';
      }
      if(link.parameter)
      {
        s += $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][parameter="'+ link.parameter +'"]').closest('label').text();
      }
      else
      {
        var input = $('input[factory="'+recipient+'"][value="'+ link.recipient_id +'"][noparam]');
        s += input.closest('label').text();
      }
    }
  }
  $('#recipients_filter_'+ recipient).html(s);
}


// Selection de filtre
function change_filter_recipient()
{
  var recipient = $(this).attr('factory');
  // mise a jour de l objet filter
  update_filters_field(recipient, filters, $(this));
  //update_hiddenfilters_field(recipient, filters);
  update_filters_preview(recipient, filters);
  $('#clearFilters').removeClass('disabled');
  $('#launchFilter').removeClass('disabled');
  $('#allSectionLaunchFilter').removeClass('disabled');
}

function filter()
{

  var text=$(this).val().toUpperCase();
  var elements=$(this).parent().find('label');
  if(text != '')
  {
    
    elements.each(function(){
      var txt = $(this).text().toUpperCase();
      $(this).parent().removeClass('search-visible');
      $(this).parent().removeClass('search-invisible');
      if(txt.indexOf(text) !== -1)
      {
        $(this).parentsUntil('div.modal_body').addClass('search-visible');
      }
      else
      {
        $(this).parent().addClass('search-invisible');
      }
    });
    
  }
  else
  {
    elements.each(function(){
      var parent = $(this).parentsUntil('div.modal_body');
      parent.removeClass('search-visible');
      parent.removeClass('search-invisible');
    });
  }
  
}


$(function(){
// DEB AJOUT TERMINUS 2.2
  $('.openclose').click(openclose);
  $('[data-type=show_recipients]').click(print_recipients);
  $('input[action=search]').keyup(filter);
  $('input[factory]').change(change_filter_recipient);
  $('#mFilters a[factory]').click(show_recipients_click);
});