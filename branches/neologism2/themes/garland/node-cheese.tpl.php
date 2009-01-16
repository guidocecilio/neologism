<?php
// $Id: node.tpl.php,v 1.5 2007/10/11 09:51:29 goba Exp $
?>

<? 
// TODOmove this somewhere else
// build the typeof class name.

$rdfa_typeof = 'drupal:' . _build_site_rdf_class_id($node->type);

?>

<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?>" typeof="<?php print $rdfa_typeof ?>" about="<?php print $node_url ?>#self">
<?php print $picture ?>

<?php if ($page == 0): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>

  <?php if ($submitted): ?>
    <span class="submitted"><?php print $submitted; ?></span>
  <?php endif; ?>

  <div class="content clear-block">
    <?php print $content ?>
  </div>

  <div class="clear-block">
    <div class="meta">
    <?php if ($taxonomy): ?>
      <div class="terms"><?php print $terms ?></div>
    <?php endif;?>
    </div>

    <?php if ($links): ?>
      <div class="links"><?php print $links; ?></div>
    <?php endif; ?>
    
    <div class="links">
      <?php if ($teaser): ?>
       <a href="<? print $node_url ?>">Read the reviews</a>
      <?php endif; ?>
      <?php if ($teaser && $logged_in): ?>
        - 
      <?php endif; ?>
      <?php if ($logged_in): ?>
        <a href="<? print url('node/add/cheese-review') ?>">Write a review</a>
      <?php endif; ?>

    </div>
    
  </div>

</div>
