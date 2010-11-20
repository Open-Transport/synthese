
// Interactivity

function getDiv(image)
{
    var div;
    for (div=image.nextSibling; div && div.tagName !='DIV'; div=div.nextSibling);
    return div;
}

function showHideNode(image)
{
    var div = getDiv(image);
    if (!div)
        return;
    if (div.className=='hiddensubnode')
        show(image, div);
    else
        hide(image, div);
}


function show(image, div)
{
    image.src = (image.src.substring(image.src.length-17) == 'ftv2plastnode.png') ? "ftv2mlastnode.png" : 'ftv2mnode.png';
    div.className= 'visiblesubnode';
}

function hide(image, div)
{
    image.src = (image.src.substring(image.src.length-17) == 'ftv2mlastnode.png') ? "ftv2plastnode.png" : 'ftv2pnode.png';
    div.className= 'hiddensubnode';
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
