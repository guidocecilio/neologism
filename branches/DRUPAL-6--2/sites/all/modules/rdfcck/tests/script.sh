mkdir out

curl -s http://drupal.deri.ie/cheese/ | python /Users/stecor/htdocs/pyrdfa/pyRdfa/scripts/localRDFa.py -n | sort > out/localrdfa_drupal.deri.ie-cheese

curl -s http://drupal.deri.ie/cheese/node/20 | python /Users/stecor/htdocs/pyrdfa/pyRdfa/scripts/localRDFa.py -n | sort > out/localrdfa_drupal.deri.ie-cheese-node-20

curl 'http://drupal.deri.ie/cheese/ns#' | sort > out/_drupal.deri.ie-cheese-ns

curl 'http://www.w3.org/2007/08/pyRdfa/extract?uri=http%3A%2F%2Fdrupal.deri.ie%2Fcheese%2F&format=nt&warnings=false&parser=lax&host=xhtml&space-preserve=true&submit=Go!' | sort > out/w3crdfa_drupal.deri.ie-cheese

curl 'http://www.w3.org/2007/08/pyRdfa/extract?uri=http%3A%2F%2Fdrupal.deri.ie%2Fcheese%2Fnode%2F20&format=nt&warnings=false&parser=lax&host=xhtml&space-preserve=true&submit=Go!' | sort > out/w3crdfa_drupal.deri.ie-cheese-node-20
