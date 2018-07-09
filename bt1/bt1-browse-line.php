<?php
	ini_set('display_errors', true);
	class TableCell {
		var $address, $row;
	}
	class Table {
		var $rowCnt, $colCnt;
		var $consensus = array();
		var $id;
		var $columns;

		function load($f) {
			fscanf($f, "%d%d %[^\n]", $this->colCnt, $this->rowCnt, $line);
			if (!$f) return false;
			$ss = explode(" ", $line);
			#echo "L=$line";
			for ($i=0; $i<$this->rowCnt; $i++)
				$this->consensus[] = $ss[$i];
			#print_r($this->consensus);
			if (count($ss) < $this->rowCnt+1) {
				echo "Error!".$ss." ".$this->rowCnt;
			}
			$this->id = $ss[$this->rowCnt];
			for ($i=0; $i<$this->colCnt; $i++) {
				$s = fgets($f);
				$ss = explode(" ", $s);
				$this->columns[$i] = array();
				for ($j=0; $j<count($ss); $j++) 
					if (trim($ss[$j])) {
						$c = new TableCell;
						list($c->address, $c->row) = sscanf($ss[$j], "(%[^)])|%d");
						$this->columns[$i][] = $c;
					}
			}
			#echo "C=".$this->id;
			return true;
		}

		function pConsensus() {
			$s = "";
			for ($j=0; $j<count($this->consensus); $j++)
				$s .= untransliterate($this->consensus[$j])." ";
			return $s;
		}

		function htmlConsensus() {
			global $rootSampleIndex;
			$s = "";
			for ($j=0; $j<count($this->consensus); $j++)
				$s .= "<li class='cnsWord' title='".untransliterate($rootSampleIndex[$this->consensus[$j]])."' wroot='".$this->consensus[$j]."'>" . untransliterate($this->consensus[$j]). "</li> ";
			return $s;
		}

		function tConsensus() {
			$s = "";
			for ($j=0; $j<count($this->consensus); $j++)
				$s .= $this->consensus[$j]." ";
			return $s;
		}

		function drawTable(){
			echo "<table class='tbl'>\n";
			foreach ($this->columns as $colIdx => $col) {
				echo "<tr>\n";
				$first = $col[0];
				$last = $col[count($col)-1];

				list($currentSure, $currentAye, $currentWord) = explode(":", $first->address);
				$currentWord = 1;
				for ($r=0; $r<$this->rowCnt+1; $r++) {
					for ($cfIdx = 0; $cfIdx < count($col) && $col[$cfIdx]->row < $r; )
						$cfIdx += 1;
					if ($cfIdx < count($col)) 
						list($fSure, $fAye, $fWord) = explode(":", $col[$cfIdx]->address);
					else {
						//echo "LAST:" . $last->address;
						//print_r ($col);
						list($fSure, $fAye, $fWord) = explode(":", $last->address);
						$fAye += 1;
						$fWord = 1;
					}
					echo "\t<td>";
					while ($currentAye != $fAye or $currentWord != $fWord) {
						if ($currentWord <= ayeLen($currentSure, $currentAye)) {
							echo "<img src='data/images-lg/" . corpusWordToImage($currentSure, $currentAye, $currentWord) . ".png'/> ";
						} else {
							echo " (" . $currentAye . ") ";
						}
						//echo "<$currentAye $currentWord>";
						if ($currentWord == ayeLen($currentSure, $currentAye) + 1) {
							$currentWord = 1;
							$currentAye += 1;
						} else 
							$currentWord += 1;
					}
					echo "\n\t<td class='M$r'>";
					if ($cfIdx < count($col) && $col[$cfIdx]->row == $r) {
						//echo "M:<$currentAye $currentWord>";
						echo "<img src='data/images-lg/" . corpusWordToImage($currentSure, $currentAye, $currentWord) . ".png'/>";
						if ($currentWord == ayeLen($currentSure, $currentAye) + 1) {
							$currentWord = 1;
							$currentAye += 1;
						} else 
							$currentWord += 1;
					}
					echo "\n";
				}
			}
			echo "</table>\n";
		}
	};

// http://corpus.quran.com/java/buckwalter.jsp
	function untransliterate($s) {
		#return $s;
		$translit='AbtvjHxd*rzs$SDTZEgfqklmnhwy\'><&}|{`YauiFNK~op_[^';
		$text=array('ا', 'ب', 'ت', 'ث', 'ج', 'ح', 'خ', 'د', 'ذ', 'ر', 'ز', 'س', 'ش', 'ص', 'ض', 'ط', 'ظ', 'ع', 'غ', 'ف', 'ق', 'ک', 'ل', 'م', 'ن', 'ه', 'و', 'ي', 
				'ء',json_decode('\u0623'), '', '', '', '', '', '', '',
				'َ' , 'ُ', 'ِ', 'ً', 'ٌ', 'ٍ', 'ّ', 'ْ', '', '',
				'', json_decode('\u0653'));
		return str_replace(str_split($translit), $text, $s);
	}


	$corpusWordIndex = array();
	$ayeLenIndex = array();
	$rootSampleIndex = array();

	function ayeLen($s, $a) {
		global $corpusWordIndex, $ayeLenIndex;
		$ayeHash = (int)($a) + 300 * (int)($s);
		return $ayeLenIndex[$ayeHash];
	}

	function corpusWordToImage($s, $a, $w) {
		global $corpusWordIndex, $ayeLenIndex;
		$locHash = (int)($w) + 200 * (int)($a) + 200 * 300 * (int)($s);
		return $corpusWordIndex[$locHash];
	}

	function loadCorpus() {
		global $corpusWordIndex, $ayeLenIndex, $rootSampleIndex;
		$h_corpus = fopen('data/quranic-corpus-morphology-0.4.txt', 'r');
		$imageIndex = 1;
		while ($l = fgets($h_corpus)) {
			if (strlen($l) == 0 || $l[0] != '(') continue;
			sscanf($l, "%s%s%s%s", $loc, $form, $tag, $features);
			list($s, $a, $w, $sw) = explode(":", substr($loc, 1, -1));
			$locHash = (int)($w) + 200 * (int)($a) + 200 * 300 * (int)($s);
			if (!isset($corpusWordIndex[$locHash])) {
				$corpusWordIndex[$locHash] = $imageIndex;
				$imageIndex += 1;
			}
			$ayeHash = (int)($a) + 300 * (int)($s);
			if (!isset($ayeLenIndex[$ayeHash]))
				$ayeLenIndex[$ayeHash] = 0;
			$ayeLenIndex[$ayeHash] = max($ayeLenIndex[$ayeHash], (int)($w));

			if ($sw == 1) {
				if (isset($currentRoot))
				$rootSampleIndex[$currentRoot] = $currentWord;
				$currentWord = '';
				unset($currentRoot);
			}
			if ($tag != 'CONJ')
				$currentWord .= $form;
			$root = strstr($features, 'ROOT:');
			if ($root) {
				$root = substr($root, 5, strpos($root, '|')-5);
				$lem = strstr($features, 'LEM:');
				$lem = substr($lem, 4, strpos($lem, '|')-4);
				$currentRoot = $root;
			}
		}
		fclose($h_corpus);
		#print >>sys.stderr, imageIndex
	}

	function loadTables($f) {
		$res = array();
		fscanf($f, "%d", $subCnt);
		for ($i=0; $i<$subCnt; $i++) {
			$t = new Table;
			$t->load($f);
			$res[] = $t;
			#print $t->consensus[0];
		}
		return $res;
	}

	function printTables($ts, $title) {
		echo "<div class='dropdown'><a class='account'>$title</a><div class='submenu'><ul class='root'>";
		//echo "<li >$title\n<ul class='sub_menu'>";
#echo "<div calss='cnsList' style='text-align: center; align: center; margin: 0px auto 0px auto; overflow: scroll; overflow-y: hidden; white-space:nowrap; height: 3em; '>";
		for ($i=0; $i<count($ts); $i++) {
			echo "<li class='cnsBox'>";
			echo "<a href=\"bt1-browse-line.php?consensus=".$ts[$i]->tConsensus()."\">".$ts[$i]->htmlConsensus()."(". count($ts[$i]->columns)  .")</a>";
		}
		echo "</ul></div></div>\n";
#echo "</div>";
	}

	if (isSet($_GET['consensus'])) {
		$consensus = $_GET['consensus'];
	} else {
		$consensus="Alh gfr twb wly rAy";
	}
	loadCorpus();
	$descriptorspec = array(
		0 => array("pipe", "r"), // stdin
		1 => array("pipe", "w"), // stdout
		2 => array("pipe", "a") // stderr
		);

	$process = proc_open("./bt1-consensus-ss", $descriptorspec, $pipes, "");
	fwrite($pipes[0], $consensus);
	fclose($pipes[0]);

	$thisTable = new Table;
	$thisTable->load($pipes[1]);
	$subTables = loadTables($pipes[1]);
	$superTables = loadTables($pipes[1]);
	$siblingTables = loadTables($pipes[1]);

	#$tables = array();
	#$f = fopen('res-bt1/out-mrgd', 'r');
	#while (true) {
	#	$t = new Table;
	#	echo "Table newed";
	#	if (!$t->load($f)) break;
#echo "loaded " + $t;
#$tables[] = $t;
#print_r($t);
	#}
	#echo "Closing f";
	#fclose($f);

?>
<head> 
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />

<link rel="stylesheet" href="css/style.css" type="text/css" media="screen, projection"/>
<link rel="stylesheet" href="css/style-table.css" type="text/css" media="screen, projection"/>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.6.2/jquery.min.js"></script>
<link rel="stylesheet" href="http://code.jquery.com/ui/1.10.3/themes/smoothness/jquery-ui.css" />
<script src="http://code.jquery.com/jquery-1.9.1.js"></script>
<script src="http://code.jquery.com/ui/1.10.3/jquery-ui.js"></script>

<script type="text/javascript" >
$(document).ready(function()
{

$(".account").click(function()
{
var X=$(this).attr('id');
if(X==1)
{
$(".submenu", $(this).parent()).hide();
$(this).attr('id', '0');
}
else
{
$(".submenu", $(this).parent()).show();
//$(".submenu", this).each(function(){console.log(this);});
//jQuery(".submenu", this).show();
//$(".submenu").show();
$(this).attr('id', '1');
}
var l = this;
$(".submenu").each(function() {
	//console.log(this + ' ' + l);
	if ($(this).parent()[0] !== $(l).parent()[0]) {
		//console.log(this + ' ' + l + ' != ' );
		//console.log($(this).parent(), $(l).parent());
		$(this).hide();
		$(this).parent().find('.account').attr('id', '');
	}
});

});

//Mouse click on sub menu
$(".submenu").mouseup(function()
{
return false
});

//Mouse click on my account link
$(".account").mouseup(function()
{
return false
});


//Document Click
$(document).mouseup(function()
{
$(".submenu").hide();
$(".account").attr('id', '');
});
});

$( function() {
		$('#consensus .cnsBox').sortable( {
update: function(event, ui) {
var v = '';
$(ui.sender).find('li').each(function() { 
v += $(this).attr('wroot');
});
console.log(v);
}

			});
		});
</script>

<style>
</style>
<title><?php echo untransliterate($consensus); ?></title>
</head>
<body>

<div id='consensus' style='text-align: center; align: center; margin: 0px auto 0px auto; '>
<div class='cnsBox'>
<?php
	echo $thisTable->htmlConsensus();
?>
</div>
</div>
<table cellpadding="10px"><tr>
<?php
	echo "<td>";
	printTables($subTables, 'زير جدول‌ها');
	echo "<td>";
	printTables($superTables, 'ابر جدول‌ها');
	echo "<td>";
	printTables($siblingTables, 'جدول‌هاي موازي');
	
	#echo "Finished";
?>
</table>

<div class='outerTable'>
<div class='innerTable' >
<?php
	if ($thisTable->colCnt > 0)
		$thisTable->drawTable();

?>
</div>
</div>

