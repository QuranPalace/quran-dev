var quran = {};
function init() {
	$.each(QuranData.SuraLength, function (i, q) {
		if (q == undefined) 
			return;
		if (q > 1)
			$('#sure-' + i).height(q * 15 * 2 + 'em');
		else
			$('#sure-' + i).height(QuranData.Sura[i][1] * 2 + 'em');
	});
}

/*
var visibleY = function(el){
	var rect = el.getBoundingClientRect(), top = rect.top, height = rect.height, 
	    el = el.parentNode;
	do {
		rect = el.getBoundingClientRect();
		if (top <= rect.bottom === false) return false;
		// Check if the element is out of view due to a container scrolling
		if ((top + height) <= rect.top) return false;
		el = el.parentNode;
	} while (el != document.body);
	// Check its within the document viewport
	return top <= document.documentElement.clientHeight;
};
*/

function isScrolledIntoView(elem) {
	var docViewTop = $(window).scrollTop();
	var docViewBottom = docViewTop + $(window).height();

	var elemTop = $(elem).offset().top;
	var elemBottom = elemTop + $(elem).height();

	return !((elemBottom <= docViewTop) || (elemTop >= docViewBottom));
}

function fillSure(sure) {
	e = $('#sure-'+sure);
	//if (e.html().trim().length != 0)
	//	return;
	e.empty();
	for (var i = 0; i < quran[sure].aya.length; i++) {
		//e.append("[" + QuranData.Sura[sure][4] + i + "] ");
		s = "<span id='aye-"+(sure)+"-"+(i+1)+"'>";
		s += quran[sure].aya[i]["@attributes"].text;

		//e.append("<br>");
		s += " (" + (i+1) + ") ";

		s += "</span>";

		e.append(s);
	}
	e.css('height', 'auto');
}

function loadSure(sure, done) {
	if (quran[sure] != undefined)
		return;
	quran[sure] = {}
	e = $('#sure-'+sure).html('LOADING ...');
	$.getJSON("http://212.16.76.108/~hadi/quran/view/quran.php?sure="+sure)
	.done(function(json){
		console.log('GET Sure ' + sure, json);
		quran[sure] = json;
		fillSure(sure);
		if (done != undefined)
			done();
	})
	.fail(function (o, status, error) {
		console.log('error', error);
	});
}

highlight = function(node, windex) {
	function innerHighlight(node, windex) {
		var skip = 0;
		//if (node.nodeType == 3) {
			var pos = 0;
			var text = node.text();
			for (i=0; i<windex-1; i++) {
				var idx = text.indexOf(' ', pos);
				if (idx != -1)
					pos = idx+1;
			}
			var pos_e = text.length;
			var idx_e = text.indexOf(' ', pos);
			if (idx_e != -1)
				pos_e = idx_e;
			if (pos >= 0) {
				var spannode = $('<span/>');
				spannode.className = 'highlight';
				//var middlebit = node.splitText(pos);
				//var endbit = middlebit.splitText(pos_e - pos);
				//var middleclone = middlebit.cloneNode(true);
				spannode.append(text.substr(pos, pos_e - pos));
				//middlebit.parentNode.replaceChild(spannode, middlebit);
				node.empty();
				node.append(text.substr(0, pos));
				node.append(spannode);
				node.append(text.substr(pos_e));
				skip = 1;
			}
			/*
		}
		else if (node.nodeType == 1 && node.childNodes && !/(script|style)/i.test(node.tagName)) {
			for (var i = 0; i < node.childNodes.length; ++i) {
				i += innerHighlight(node.childNodes[i], windex);
			}
		}
		*/
		return skip;
	}
	return innerHighlight(node, windex);
};

$(document).ready(function(){
	/*
	*/
	QuranData.SuraLength = [];
	for (var i=1; i<=114; i++)
		QuranData.SuraLength[i] = 0;
	for (var i=1; i<QuranData.Page.length-1; i++) { //-1 is for bad data!
		QuranData.SuraLength[QuranData.Page[i][0]]++;
	}
	for (var i=1; i<=114; i++)
		if (QuranData.SuraLength[i] == 0)
			QuranData.SuraLength[i] = 1;
	init();
	//$(document).scroll(function () {
		console.log('Scroll');
		for (var i=1; i<=114; i++) {
			var s = $('#sure-'+i);
			//if (s.visible(true)) {
			//if (isScrolledIntoView(s)) {
				console.log("View " + i);
				loadSure(i);
			//}
		}
	//});

	if (window.location.href.search('#') != -1) {
		var h = window.location.href;
		h = h.substr(h.search('#')+1);
		r = /(\d+):?(\d+)?:?(\d+)?/;
		m = h.match(r);
		var s = m[1] == undefined ? 1 : m[1];
		var a = m[2] == undefined ? 1 : m[2];
		var w = m[3] == undefined ? 1 : m[3];
		if (a == undefined)
			$.scrollTo($('#sure-'+s));
		else  {
			loadSure(s, function() {
				e = $('#aye-'+s+'-'+a);
				$.scrollTo(e);
				e.addClass("aye-highlight");
				if (w != undefined) {
					//highlightWord(e, w);
				}
			});
		}
	}

});
