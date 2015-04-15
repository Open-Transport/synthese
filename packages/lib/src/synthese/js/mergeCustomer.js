$( document ).ready(function() {
	/* For each row in the table */
	$(".propertysheet").find("tbody").find("tr").each(function(index) {
		/* Check we are looking at interesting content */
		if ($(this).has("td").length)
		{
			var input = $(this).find("input");
			if (!input.length)
				input = $(this).find("textarea");
			var comparaison = $(this).find("td:last-child");	
			
			/* Ugly hack to avoid putting an arrow in last row */
			if (comparaison.html() !== ( "oui" || "non"))
			{
				if (input.val().length)
				{
					if (comparaison.html().length && input.val() !== comparaison.html() )
					{
						$(this).find("td:nth-child(3n)").append("<a class='putleft' style='background:url(/admin/img/arrow_left.png) no-repeat transparent;height:22px;width:25px;display:block;cursor:pointer;'></a>");
						input.css("border","2px red solid");
						$(this).find("td:nth-child(3n)").on("click", function(event) {
							event.preventDefault();
							input.val(comparaison.html());
							input.css("border","2px inset");
							$(this).empty();
						});
						input.on("change", function(event) {
							input.css("border","2px inset");
						});
					}
				}
				else if (comparaison.html().length)
				{
					input.css("border","2px green solid");
					input.val(comparaison.html());
					$(this).find("td:nth-child(3n)").append("<div class='switch' style='background:url(/admin/img/arrow_switch.png) no-repeat transparent;height:20px;width:20px;display:block;transform: rotate(180deg);-webkit-transform: rotate(180deg);-moz-transform: rotate(180deg);-ms-transform: rotate(180deg);-o-transform: rotate(180deg);'></div>");
				}
			}
		}
	});
});