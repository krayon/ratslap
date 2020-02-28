# Contributing #

## Making Changes ##

Wherever possible, please base your changes on the *develop* branch. The one
exception is if you have a fix for a security issue in a specific release in
which case base it off that release branch. Either way, don't be too concerned
if you can't decide what to branch off, an incorrectly targeted PR is better
than no PR at all :D

... and Thank you.

## Basic workflow ##

  1. Go to the project page, and select *Fork*
  2. Clone your copy of the repository:
```
    git clone https://gitlab.com/<YOURUSERNAME>/ratslap
```
  3. Check out the *develop* branch as your new feature branch:
```
    git checkout -b feature/coolnewfeature origin/develop
```
  4. Make your changes and commit them to your branch.
  5. Push your branch up to your repository:
```
    git push origin feature/coolnewfeature:feature/coolnewfeature
```
  6. Create a Pull Request (PR - GitHub)/Merge Request (MR - GitLab) with
     *develop* as the base/destination.



----
##### vim:set ts=4 sw=4 tw=80 et cindent ai si syn=markdown: #####
