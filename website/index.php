<?php

/*
TODO
- Test in Firefox
- Test in Chrome
- Test in Opera
- Test in Internet Explorer
- Add Google Analytics
- Deploy
- Layout for Showcase page
- Better screenshot for homepage
- Improve screenshot display on homepage
- HTML validation
- Tons of content
- RDFa markup
*/

// ============= SITE STRUCTURE AND PAGES  ==========================================

$main_links = array(
    'Home' => '',
    'Showcase' => 'showcase',
    'About' => 'about',
    'Docs' => 'docs',
    'Support & Devel' => 'support-dev',
);

$section_links = array(
    'docs' => array(
        'Overview' => 'docs',
        'Minimum requirements' => 'requirements',
        'Installation guide' => 'installation',
        'RDFS and OWL' => 'on-vocabularies',
        'FAQ' => 'faq',
        'Publications' => 'publications',
    ),
    'support-dev' => array(
        'Mailing list' => 'mailing-list',
        'FAQ' => 'faq',
        'Issue tracker' => 'issues',
        'Source code' => 'source',
    ),
);

$pages = array(
    '' => array(
        'head_title' => 'Neologism – Easy Vocabulary Publishing',
        'title_content' => 'home-intro',
        'content' => 'home-text',
    ),
    'showcase' => array(
        'title' => 'Showcase',
        'text' => 'showcase',
    ),
    'about' => array(
        'title' => 'About Neologism',
        'text' => 'about',
    ),
    'docs' => array(
        'title' => 'Documentation',
        'text' => 'docs',
    ),
    'support-dev' => array(
        'title' => 'Support and Development',
        'text' => 'support-dev',
        'sidebar' => 'team',
    ),
    'requirements' => array(
        'title' => 'Minimum Requirements',
        'section' => 'docs',
        'text' => 'requirements',
    ),
    'installation' => array(
        'title' => 'Neologism Installation Guide',
        'section' => 'docs',
        'text' => 'installation',
    ),
    'on-vocabularies' => array(
        'title' => 'On Vocabularies, RDFS, and OWL',
        'section' => 'docs',
        'text' => 'on-vocabularies',
    ),
    'faq' => array(
        'title' => 'Frequently Asked Questions',
        'section' => 'docs',
        'text' => 'faq',
    ),
    'publications' => array(
        'title' => 'Academic Publications about Neologism',
        'section' => 'docs',
        'text' => 'publications',
    ),
    'gpl' => array('redirect' => 'http://www.gnu.org/licenses/gpl-3.0-standalone.html'),
    'download' => array('redirect' => 'http://code.google.com/p/neologism/downloads/list'),
    'download-latest' => array('redirect' => 'http://neologism.googlecode.com/files/neologism-1.0-rc5.zip'),
    'mailing-list' => array('redirect' => 'http://groups.google.com/group/neologism-dev'),
    'issues' => array('redirect' => 'http://code.google.com/p/neologism/issues/list'),
    'source' => array('redirect' => 'http://code.google.com/p/neologism/source/checkout'),
);

// ============= BEHAVIOUR CODE ==========================================

$uri = $_SERVER['REQUEST_URI'];
$path = str_replace('index.php', '', $_SERVER['SCRIPT_NAME']);
$absolute_base = 'http://' . $_SERVER['HTTP_HOST'] . $path;

if (substr($uri, 0, strlen($path)) == $path) {
    $uri = substr($uri, strlen($path));
}
if (!$uri) {
    $uri = '';
}
if (!isset($pages[$uri])) {
    $section = 'none';
    $title = '404 Not Found';
    $content = '404';
    header("HTTP/1.0 404 Not Found");
} else if (isset($pages[$uri]['redirect'])) {
    $section = 'none';
    $target = $pages[$uri]['redirect'];
    $title = 'Redirect';
    $text_html = '<p>The requested content is found at the following location: <a href="'
            . htmlspecialchars($target) . '">' . htmlspecialchars($target) . '</a></p>';
    header("Location: $target");
    header("HTTP/1.0 302 Found");
} else {
    $section = $uri;
    foreach ($pages[$uri] as $varname => $var) {
        $$varname = $var;
    }
    $page_links = @$section_links[$section];
}

// ============= TEMPLATE FUNCTIONS ==========================================

function e($s) { echo htmlspecialchars($s); }
function content($file) { readfile("content/$file.html"); }

// ============= BEGIN TEMPLATE ==============================================

echo '<?xml version="1.0" encoding="UTF-8"?>';
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML+RDFa 1.0//EN"
    "http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
  <head>
    <title><?php e(@$head_title ? $head_title : ($title . ' | Neologism')); ?></title>
    <base href="<?php e($absolute_base); ?>" />
    <link rel="stylesheet" type="text/css" href="style.css" />
    <link rel="shortcut icon" type="image/png" href="images/favicon.png" />
  </head>
  <body>
    <div id="header">
      <<?php e(($uri == '') ? 'h1' : 'div'); ?> id="logo"><a href=""><img src="images/neologism-logo-300.png" alt="Neologism" /></a></<?php e(($uri == '') ? 'h1' : 'div'); ?>>
      <ul id="main-nav">
<?php
foreach ($main_links as $label => $link) {
    if ($section == $link) {
?>
        <li class="active"><span><?php e($label); ?></span></li>
<?php
    } else {
?>
        <li><a href="<?php e($link); ?>"><?php e($label); ?></a></li>
<?php
    }
}
?>
        <li class="download"><a href="download">Download</a></li>
      </ul>
    </div>
    <div id="title-stripe">
      <div id="title">
<?php
if (@$title) {
    ?><h1><?php e($title); ?></h1><?php
} else if (@$title_content) {
    content($title_content);
} else {
    echo "<h1>No title</h1>";
}
?>
      </div>
    </div>

    <div id="content">
<?php if (@$page_links) { ?>
      <ul id="section-nav">
<?php
foreach($page_links as $label => $link) {
    if ($link == $uri) { ?>
        <li><?php e($label); ?></li>
<?php } else { ?>
        <li><a href="<?php e($link); ?>"><?php e($label); ?></a></li>
<?php
    }
}
?>
      </ul>
<?php
}
if (@$sidebar) {
    ?><div id="sidebar"><?php content($sidebar); ?></div><?php
}
if (@$content) {
    content($content);
} else if (@$text) {
    ?><div id="main"<?php if (@$page_links) { ?> class="centrecol"<?php } ?>><?php content($text); ?></div><?php
} else if (@$text_html) {
    echo $text_html;
} else {
    echo "<p>No content or text configured for this page.</p>";
}
?>
    </div>

    <div id="footer">
      <div id="footer-text">
        <ul id="footer-links">
          <li><a href="about">About</a></li>
          <li><a href="http://linkeddata.deri.ie/">Linked Data Research Centre</a></li>
          <li><a href="http://deri.ie/">DERI</a></li>
          <li><a href="http://www.nuigalway.ie/">National University of Ireland, Galway</a></li>
          <li class="last"><a href="gpl">GPL</a></li>
        </ul>
        <div id="tagline">Easy Vocabulary Publishing</div>
      </div>
    </div>
  </body>
</html>
