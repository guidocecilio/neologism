# Adding pages with further information about a vocabulary #

For complex vocabularies, you may want to include a lot of content, such as examples, related tools, usage notes and so on. Here I describe how to move such content from the main vocabulary page to separate pages.

# Creating sub-pages for a vocabulary #

  1. Log in and go to _Create Content » Page_.
  1. Give the page a title and insert its content.
  1. Optionally, expand the _URL path settings_ section of the page editing form and enter a path that is “below” the vocabulary, such as `myvocab/mypage`.
  1. Link to the page from the main vocabulary page using a standard HTML link.

# Creating a menu of the sub-pages related to a vocabulary #

You can also create a dedicated navigation menu that lists all the pages related to one vocabulary. This requires the menu\_block module, so you need to be able to install modules and have administrator privileges.

  1. Install and enable the [menu\_block](http://drupal.org/project/menu_block) module
  1. In _Administer » Site building » Menus_, go to the _Add menu_ tab and create a menu with name `vocabularies` and title “Vocabulary menu”.
  1. Add an item “myvocab” with link to “myvocab”
  1. Add more items as children of this item, for example “MyVocab Specification” with link “myvocab”, “Examples” with link “myvocab/examples”, and “Wiki” with link to “http://wiki.example/com/”.
  1. Note that you can also add pages to menus from the page editing form, by expanding the “Menu settings” section.
  1. In _Administer » Site building » Blocks_, click the “New menu block” button.
  1. Select the following options:
    * Title: “See also”
    * Parent item: “Vocabulary menu” / “Root of Vocabulary menu”
    * Starting level: “2nd level (secondary)”
  1. After the block is created, assign it to the right sidebar in the _Blocks_ admin screen.

This will show the menu in the right sidebar whenever you are on a page that belongs to the menu.

Optionally, you can set the _Page-specific visibility settings_ of the block to _Show if the following PHP code returns true_, and paste the following code:

```
<?php
module_load_include('module', 'menu_block');
$block = _menu_block_block_view(1);
return substr_count($block['content'], '<li') > 1;
?>
```

This will hide the menu if your vocabulary does not have at least two associated menu items, to prevent the showing of a menu that contains only a single item.