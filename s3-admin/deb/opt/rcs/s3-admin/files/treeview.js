
// Interactivity

function getDiv(image)
{
    var div=image.parentNode;
    div=div.nextSibling;
    div=div.nextSibling;
    if(div.nextSibling != null)
    {
       div=div.nextSibling;
       div=div.nextSibling;
    }
    return div;
}

function showHideNode(image)
{
    var div = getDiv(image);
    if (!div)
        return;
    if (div.style.display=='none')
        show(image, div);
    else
        hide(image, div);
}


function show(image, div)
{
    image.src = "ftv2mlastnode.png";
    div.style.display = 'block';
}

function hide(image, div)
{
    image.src = "ftv2plastnode.png";
    div.style.display = 'none';
}

function showHide()
{
    showHideNode(this);
}

// Control by id (the div must have an id)

function getParentImage(image)
{
    var parent = image.parentNode;
    var parentImage;
    for (parentImage = parent; parentImage && parentImage.tagName !='IMG' && parentImage.className != 'node'; parentImage=parentImage.nextSibling);
    return parentImage;
}

function recursiveShow(image)
{
    for (; image; image = getParentImage(image))
        show(image, getDiv(image));
}

function recursiveShowByDivId(id)
{
    for (image = document.getElementById(id).previousSibling; image && image.tagName != 'IMG' && image.className != 'node'; image = image.previousSibling);
    if (image)
        recursiveShow(image);
}

// Loader

function declareTreeEvents()
{
    var images = document.getElementsByTagName("IMG");
    for (var i=0; i<images.length; ++i)
        if (images[i].className == "node")
            images[i].onclick = showHide;
}

window.onload = declareTreeEvents;
