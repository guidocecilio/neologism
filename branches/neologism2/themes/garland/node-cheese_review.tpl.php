<?php
// $Id: node.tpl.php,v 1.5 2007/10/11 09:51:29 goba Exp $
?>

<? 
// TODOmove this somewhere else
// build the typeof class name.

$rdfa_typeof = 'drupal:' . _build_site_rdf_class_id($node->type);
$rdfa_title = 'drupal:' . _build_site_rdf_property_id('title', $node->type);
$rdfa_child_node_property = 'drupal:' . _build_site_rdf_property_id('review', 'cheese');;
?>

<div rel="<?php print $rdfa_child_node_property; ?>" >

<div id="node-<?php print $node->nid; ?>" class="node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?>" typeof="<?php print $rdfa_typeof ?>" about="<?php print $node_url ?>#self">
<?php print $picture ?>

<?php if ($page == 0): ?>
  <h2><a property="<?php print $rdfa_title ?>" rel="foaf:page" href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>

<?php if ($page != 0): ?>
  <span property="<?php print $rdfa_title ?>" content="<?php print $title ?>"></span>
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
    
  </div>

</div>
<?php
// encapsulation for the child element cheese_review
?>
</div>