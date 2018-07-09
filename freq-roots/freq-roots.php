<?php
include ('../search/inc.php');

$sure=isset($_GET['sure']) ? $_GET['sure'] : 1;
$limit=isset($_GET['limit']) ? $_GET['limit'] : 100;

$link = db_connect();
$result = mysqli_query($link, "select root, aye from WORD where sure=$sure") or die('Query failed: ' . mysqli_error($link));
$rootCount = [];
while ($line = mysqli_fetch_array($result, MYSQLI_ASSOC)) {
	if ($line['root'] != NULL) {
		if (!isset($rootCount[$line['root']]))
			$rootCount[$line['root']] = [];
		array_push($rootCount[$line['root']], $line['aye']);
	}
}

mysqli_free_result($result);
mysqli_close($link);

$f = file_get_contents("../data/quran-simple.xml");
$xml = simplexml_load_string($f);
$json_string = json_encode($xml, JSON_UNESCAPED_UNICODE);
$json = json_decode($json_string);
?><html>
	<head>
		<title>ریشه‌های پرتکرار</title>
		<meta charset="utf-8" />
		<meta http-equiv="X-UA-Compatible" content="IE=edge">
		<meta name="viewport" content="width=device-width, initial-scale=1">

		<link href="css/bootstrap.min.css" rel="stylesheet">

		<!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
		<!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
		<!--[if lt IE 9]>
			<script src="https://oss.maxcdn.com/html5shiv/3.7.3/html5shiv.min.js"></script>
			<script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
		<![endif]-->


    <link href="gh/css/agency.min.css" rel="stylesheet">

		<script src="jquery.min.js"></script>
		<link href="jquery.dataTables.min.css" rel="stylesheet">
		<script src="jquery.dataTables.min.js"></script>
		<script src="jquery.numeric.min.js"></script>
		<script src="quran-data.js"></script>
		<script src="main.js"></script>
		<link href="main.css" rel="stylesheet">


	</head>
	<body class="index">
		<div id="page-wrapper">

			<!-- Header -->
			<!-- Banner -->
					<div class="inner">

						<header>
							<h2>ریشه‌های پرتکرار</h2>
						</header>
					</div>

			<!-- Main -->
				<article id="main">
									<header>
										<h2>پارامترها</h2>
									</header>
					<!-- One -->
							<div id='search-q' class="row 50%" style="">
									<footer>
										<form action='freq-roots.php' method=get>
											سوره: 
											<select name="sure">
												<?php
													foreach ($json->sura as $k => $v) {
														echo "<option value='".$v->{"@attributes"}->index."' ";
														if ($v->{"@attributes"}->index == $sure)
															echo "selected";
														echo ">".$v->{"@attributes"}->index . " - " .$v->{"@attributes"}->name."</option>";
													}
												?>
											</select>
											تعداد ریشه‌ها:
												<?php
													echo "<input name='limit' value='$limit'>";
												?>
											<input type=submit value="نمایش">
										</form>
									</footer>

							</div>

					<!-- Two -->
					<!-- Three -->
					<article class='main-table'>

							<header class="major">
								<h2>نتایج</h2>
							</header>
										<div id='r'>
										</div>
							<div class="table-responsive">

						<?php
							uasort($rootCount, function ($a, $b) { 
									$ca=count($a); $cb=count($b); if ($ca == $cb) return 0; if ($ca < $cb) return +1; return -1;
									}
								);
							//print_r($rootCount);
							$freqRoots = array_slice($rootCount, 0, $limit, false);


							echo "<table id='freq-table' class='table  table-striped'>";

							echo "<thead><tr><td>";
							foreach ($freqRoots as $root => $ayeList) {
								echo "<td>".to_persian($root) . " (" . count($ayeList). ")";
							}
							echo "</thead><tbody>";
							foreach ($json->sura[$sure-1]->aya as $k => $v) {
								echo "<tr><td>";
								echo $v->{"@attributes"}->text;
								foreach ($freqRoots as $root => $ayeList) {
									echo "<td>";
									if (in_array($k+1, $ayeList))
										echo "1";
								}
							}
							echo "</table>";
						?>
							</div>
					</article>

				</article>

			<!-- CTA -->
			<!-- Footer -->
		</div>

		<!-- Scripts -->
			<script src="js/bootstrap.min.js"></script>

</body>
</html>
