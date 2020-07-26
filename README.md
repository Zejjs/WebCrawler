# WebCrawler

A basic console-based web crawler written in C without using any existing HTTP libraries.

The program is run by calling *crawler* followed by a URL

e.g. *crawler http://<span></span>www<span></span>.example.com*

The crawler will follow and return all hypertext links whose host matches all but the first component of the URL provided in the command line.

e.g. *crawler http://<span></span>www<span></span>.example.com* will follow links to http://<span></span>ww2<span></span>.example.com/<span></span>page, http://<span></span>ww3<span></span>.example.com<span></span>/page2 etc, but now http://<span></span>www<span></span>.example2.com

Fetched URLs are printed to stdout.
