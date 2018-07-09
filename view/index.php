<head>
		<title>Quran Advanced Search</title>
		<meta charset="utf-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1" />
		<script src="../search/jquery.min.js"></script>
		<script src="../search/quran-data.js"></script>
		<script src="jquery.visible.min.js"></script>
		<script src="jquery.scrollTo.min.js"></script>
		<script src="main.js"></script>
		<link href="main.css" rel="stylesheet">
		<style type="text/css"> @font-face {font-family: 'Scheheraza';src: url('http://tanzil.net/res/font/eot/Scheherazade.eot');src: local('Scheherazade'), url('http://tanzil.net/res/font/org/Scheherazade.ttf') format('truetype');} </style>
</head>
<body>
	<div id='quran'>
		<?php
			for ($i=1; $i<=114; $i++) {
				?>
				<div id='sure-<?php echo $i; ?>' class='sure'>
				</div>
				<?php
			}
		?>
	</div>
</body>
