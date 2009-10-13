<?php
// $Id: page.tpl.php,v 1.18.2.1 2009/04/30 00:13:31 goba Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <?php print $head ?>
    <title><?php print $head_title ?></title>
    
	<?php print $styles ?>
    <!--<link type="text/css" rel="stylesheet" media="all" href="style.css" />-->
    
    <?php print $scripts ?>

</head>

	<body >

<!-- Layout -->
      <div id="header-region" class="clear-block"><?php print $header; ?></div>
        <div id="wrapper"> 
        	<div id="container">
                <div id="header">	
                	<div id="logo-floater">
						<?php
                          // Prepare header
                          $site_fields = array();
                          if ($site_name) {
                            $site_fields[] = check_plain($site_name);
                          }
                          if ($site_slogan) {
                            $site_fields[] = check_plain($site_slogan);
                          }
                          $site_title = implode(' ', $site_fields);
                          if ($site_fields) {
                            $site_fields[0] = '<span>'. $site_fields[0] .'</span>';
                          }
                          $site_html = implode(' ', $site_fields);
                
                          if ($logo || $site_title) {
                            print '<a href="'. check_url($front_page) .'" title="'. $site_title .'">';
                            if ($logo) {
                              print '<img src="'. check_url($logo) .'" alt="'. $site_title .'" id="logo" />';
                            }
                            //print $site_html .'</a></h1>';
							print '</a>';
                          }
                        ?>
                    </div>
            
                    <!-- navigation -->
                  <div id="navigation">
                        <?php if (isset($secondary_links)) : ?>
                        <?php print theme('links', $secondary_links, array('class' => 'links secondary-links')) ?>
                        <?php endif; ?>
                    </div>
                </div> <!-- /header -->
                
                <div id="primary">
                	<?php if (isset($primary_links)) { ?><?php print theme('links', $primary_links, array('class' =>'links primary-links')) ?><?php } ?>
    			</div>
                
				<div id="center" class="clearfix">
                	<div id=<? if ($right) print '"content-mediumsize"'; else print '"content-fullsize"'; ?>" > 
						<?php print $breadcrumb; ?>
                        <?php if ($mission): print '<div id="mission">'. $mission .'</div>'; endif; ?>
                        <?php if ($tabs): print '<div id="tabs-wrapper" class="clear-block">'; endif; ?>
                        <!--<?php if ($title || $title == "" ): print '<h2'. ($tabs ? ' class="with-tabs"' : '') .'>'. $title .'</h2>'; endif; ?>-->
                        <?php if ($tabs): print '<ul class="tabs primary">'. $tabs .'</ul></div>'; endif; ?>
                        <?php if ($tabs2): print '<ul class="tabs secondary">'. $tabs2 .'</ul>'; endif; ?>
                        <?php if ($show_messages && $messages): print $messages; endif; ?>
                        <?php print $help; ?>
                        
                        <?php print $content ?>
                        
                        <?php print $feed_icons ?>
                    </div>
                    
                   <!-- <div id="sidebar">-->
						<?php if ($right): ?>
                            <div id="sidebar-right" class="sidebar">
                            <?php print $right ?>
                            </div>
                        <?php endif; ?>
					<!--</div>-->

              	</div> <!-- /#center -->
        	
                <div id="footer">	
                   <?php print $footer_message ?>
                   <div id="block-system-0" class="block block-system">
                     <div class="content">
                         <?php print l(theme('image', drupal_get_path('theme', 'black_imp').'/images/drupal-deri-logo.png', t('Powered by Drupal, an open source content management system.'), t('Powered by Drupal, an open source content management system.')), 'http://drupal.org', array('html' => TRUE)); ?> 
                      </div>
                   </div>        
                   <div id="bottom-wrapper"></div>
                </div>
                
            </div>  <!-- /container -->
        </div> <!-- /wrapper -->
        
        
	<!-- /layout -->

  		
    </body>
</html>

