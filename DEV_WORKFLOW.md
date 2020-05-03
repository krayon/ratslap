# Workflow #

1. master (branch) is always current;
1. develop branch is always the "root" of new activity.

## Branch Naming ##

* FEATURE: ft.*<feature>* , eg. `ft.kitchen-sink-option`
* HOTFIX:  hf.*<hotfix>*  , eg. `hf.teflon-tape`
* RELEASE: rl.*<release>* , eg. `rl.1.2.3`

## New Feature ##

### Start a new feature branch ###

```bash
git checkout -b ft.kitchen-sink-option develop
```

### Work on new feature branch ###

```bash
git checkout ft.kitchen-sink-option
# WORK WORK WORK...
git commit --gpg-sign -m 'Add stuffs'
# WORK WORK WORK...
git commit --gpg-sign -m 'Add more stuffs'
# WORK WORK WORK...
```

### Finish the feature - merge back into develop ###

```bash
git checkout ft.kitchen-sink-option
git rebase --interactive develop
# PICK first, SQUASH rest (generally)
git checkout develop
git merge --gpg-sign --no-ff ft.kitchen-sink-option
git push origin develop:develop
git push gitlab develop:develop
git push github develop:develop
# etc...
git branch -d ft.kitchen-sink-option
```

## Hotfix ##

### Start a new hotfix branch ###

```bash
git checkout -b hf.teflon-tape master
```
* ***TODO: Will this allow us to hotfix previous versions?***

### Work on new hotfix branch ###

```bash
git checkout hf.teflon-tape
# WORK WORK WORK...
git commit --gpg-sign -m 'Add stuffs'
# WORK WORK WORK...
git commit --gpg-sign -m 'Add more stuffs'
# WORK WORK WORK...
```

### Finish the hotfix - merge back into develop ###

```bash
git checkout hf.teflon-tape
# UPDATE Documentation for new hotfix etc
git add README.md
git commit --gpg-sign -m 'Hotfix: 1.0.1'
git tag -s -m 'HOTFIX: 1.0.1' 1.0.1
git checkout develop
git merge --gpg-sign hf.teflon-tape
git push --tags origin develop:develop
git push --tags gitlab develop:develop
git push --tags github develop:develop
# etc...
git branch -d hf.teflon-tape
git checkout master
git merge --ff-only 1.0.1
```

## Release ##

### Start a new release branch ###

```bash
git checkout -b rl.1.2.3 develop
# UPDATE Documentation for new version etc
git add README.md
git commit --gpg-sign -m 'Release: 1.2.3'
git tag -s -m 'RELEASE: 1.2.3' 1.2.3
git checkout develop
git merge --gpg-sign rl.1.2.3
git push --tags origin develop:develop
git push --tags gitlab develop:develop
git push --tags github develop:develop
# etc...
git branch -d rl.1.2.3
git checkout master
git merge --ff-only 1.2.3
```



----
##### vim:set ts=4 sw=4 tw=80 et cindent ai si syn=markdown: #####
