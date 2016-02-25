# Introduction #
This page describes the process for releasing a new version of Neologism, including building the new release zip file, tagging the version in SVN, and updating the website.

# Building the release #
The current version for release is the one at https://neologism.googlecode.com/svn/branches/drupal-6

  * Create an empty directory for example `neologism-d6` and change to it
```
mkdir neologism-d6
cd neologism-d6
```
  * Download the `build-release.sh` from Google SVN
```
curl -O http://neologism.googlecode.com/svn/branches/drupal-6/build-release.sh
```

  * (As an alternative to the steps above, if you already have a working copy checked out from SVN, then you can use that; it will have the script in the root directory.)
  * Check the `drush dl drupal-6.xxx` line in the script matches the latest version of Drupal 6 available at http://drupal.org/project/drupal and increase the version number if necessary
  * Run the `build-release.sh` script (you might have to do `chmod u+x build-release.sh` first)
  * Rename `neologism.zip` to `neologism-0.x.y.zip`
  * Upload the zip file to Google Code
    * Summary: "Neologism 0.x.y"
    * Labels: “Featured”
  * Edit the previous release and remove the “Featured” label. Result should be that **only** the latest release is featured on the Google Code project homepage

# Tagging the release in the SVN repository #

```
svn copy https://neologism.googlecode.com/svn/branches/drupal-6 https://neologism.googlecode.com/svn/tags/neologism-0.x.y -m 'tagging the 0.x.y release'
```

# Updating and deploying the website #

```
svn checkout https://neologism.googlecode.com/svn/trunk/website neologism-website --username richard@cyganiak.de
```

  * In index.php, search for "download-latest" and update it to the URL of the latest release zip
  * In content/home-intro.html, update the version number in the "Download Neologism 0.x.y" button
  * In content/home-text.html, add a new news item that announces the new release. News items should be 3-5 lines of text, not more! Remove the oldest news items so that there are 3 news items in total.
  * Commit changes
  * ssh into `neologism.deri.ie`, go into `/var/www/neologism.deri.ie`, run `svn up`

# Announcing the release #

  * Send an email to neologism-dev@googlegroups.com with an announcement
    * List of the most important changes
    * Upgrade instructions
  * Guido to announce on Twitter and Facebook
  * Richard to announce on Twitter
  * Remind Michael to re-tweet from the LiDRC Twitter account
  * For major releases (0.x), consider announcing on semantic-web and public-lod mailing lists, deri.ie-research, DERI blog, LiDRC blog

# Open questions/issues #

  * Changelog?