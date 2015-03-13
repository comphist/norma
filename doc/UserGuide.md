# Norma User Guide
## What is Norma?

Norma is a tool for automatic spelling normalization.  It transforms
non-standard words of some kind to standard forms, e.g., historical spelling
variants to their modern spellings.  It is a **command-line tool,** i.e., there
is no graphical interface available at this time.  The examples in this guide
will assume a Linux system, but will work similarly if you're on Windows (except
for minor details; e.g., you'd have to replace `normalize` with
`normalize.exe`).

Norma consists of several modules that can perform spelling normalization,
called **normalizers.** You can choose to use one of the supplied normalizers,
or several in combination.  If you're a developer and know C++, you can even
write your own normalizers.  (In that case, you might want to look at the
supplied Doxygen documentation of the source code.)  Typically, normalizers will
need to be trained on some manually prepared data first before they can be used
to normalize texts.  Apart from training data, most normalizers also require a
**target lexicon,** which is basically a list of all possible word forms that
the normalizers can generate.

At the moment, normalizers are restricted to work with one word at a time.  That
means they cannot take context into account or contract several words into one.
We're planning to build further functionality on top of the existing normalizers
to enable this.

If you want to cite Norma in an academic work, please use the following
reference:

> Marcel Bollmann (2012). [(Semi-)Automatic Normalization of Historical Texts using Distance Measures and the Norma tool.](http://www.linguistics.rub.de/~bollmann/pub/acrh12.pdf "PDF version")  In: Proceedings of the Second Workshop on Annotation of Corpora for Research in the Humanities (ACRH-2), pp. 3-14. Lisbon, Portugal.

## Invoking Norma

### First steps / Interactive mode

After you've installed Norma, it can be called via the `normalize` command.  If
you call Norma without any additional command-line options, you will likely get
the following message:

    Error: Normalizer chain not specified!

This is because Norma needs a *configuration file* to work, which is supplied
via the `-c` flag.  For your first steps, you can try the supplied sample
configuration:

    normalize -c example/example.cfg

This will start Norma in *interactive mode,* where you can enter wordforms and
get normalization suggestions back.  You should see a prompt like this:

    Enter any wordform to normalize it.
    Prefix a word with '!' to issue commands. For a list of all commands, type '!help'.
    Exit with CTRL+D or '!exit'.
    >

If you type in a word like *vnd* or *vrouwe* (the example configuration is set
up for Early New High German), you should get back *und* and *frau* as
normalization suggestions:

    > vnd
    und     0.992651
    > vrouwe
    frau    0.752847

The numbers after the words represent a **confidence score** that is always a
value between 0 and 1.  How exactly it is calculated differs between the
normalizer modules, but the general idea is that the higher the score, the more
reliable the normalization is considered by the system.

If you've followed these instructions and get output equal or very similar to
what is shown here: Congratulations, Norma seems to be working correctly for
you!

### Batch mode

Most of the time, you'll probably want to use Norma in *batch mode,* i.e.,
supply files for Norma to work with.  In order to **normalize all words** from a
text file that contains one wordform per line, just call:

    normalize -c example/example.cfg -f example/fnhd_sample.txt

The output will be one normalized word per line.

If you want to **train** Norma on manually normalized data, just supply a file
that contains two wordforms per line separated by a tab, then call Norma like
this:

    normalize -c example/example.cfg -f example/fnhd_train.txt -t --saveonexit

The option `-t` causes Norma to perform training only, without generating any
normalizations, while `--saveonexit` is required (if not given in the config
file) to save the trained parametrizations back to the parameter files.  You can
get a list of all possible command-line options by using the `-h` flag.

Again, you can use the supplied example files to check that everything works
correctly.

## Configuring Norma

If you're planning to use Norma with a pre-existing configuration, you probably
only need to specify the proper configuration file with the `-c` flag.  If you
want to train Norma on your own data, however, you might want to change or
create a configuration file yourself.  This section will guide you through the
necessary steps and available options.

Typically, a configuration file consists of a set of general options followed by
sections specific to the lexicon and the normalizers.  Here is a list of the
general options you can use:

* `normalizers=...` controls which normalizers to use and in what order.  This
  is probably the most important setting.  It takes a comma-separated list of
  normalizer names, e.g.:

   `normalizers=Mapper,RuleBased,WLD`

   This line indicates that the mapper, the rule-based normalizer, and the WLD
   normalizer should be used.  By changing this setting, you can choose to
   exclude some of these normalizers, use one of them in isolation, or maybe add
   a normalizer you've written yourself.  Normalizers should be given in
   descending order of *priority.* By default, when comparing candidates given
   by the various normalizers, Norma will prefer normalizers with a higher
   priority, except when the returned probability score is zero (i.e., the
   normalizer failed to find a suitable candidate).  This default cannot yet be
   changed within the configuration file.

   Note that we found the above combination and ordering of normalizers to work
   best when experimenting with historical data from Early New High German, so
   we recommend to use it by default.

* `saveonexit={False|True}` controls whether parameter files are saved when
  Norma exits.  It defaults to False.  If you want to train normalizers on your
  own training data and keep the results of that training, this should be set to
  True.  While performing normalization experiments with your newly trained
  parameter files, you'll probably want to change this to False.  Alternatively,
  you can also use a command-line switch (`--saveonexit`) instead of putting
  this in the config file.

* `perfilemode={False|True}` allows you to use Norma with many different
  parametrizations without changing the configuration file each time.  The
  option only works when an input file is given (via `-f`).  Whenever a Norma
  component expects a parameter file in the configuration but the entry is
  missing, it will use the name of the input file with a parameter-specific
  extension.

   For example, if you call Norma on the file `mytext.txt` and there is no entry
   for the lexicon's `fsmfile` (see the next section about that) in the
   configuration, Norma will use the filename `mytext.Lexicon.fsmfile` instead.

   This is particularly useful for training different parametrizations on
   different datasets, as you can use the same configuration file each time, and
   Norma will save the learned parametrizations under filenames specific to the
   input file.

The most important options in the remaining sections are the *parameter files*
of the various Norma components.  For the lexicon, there is a separate tool to
create these files (see below); for all normalizers, if the file you specify
does not exist, it will automatically be created during training.  Filenames in
the configuration file can either be given with absolute or relative paths; in
the latter case, Norma always tries to interpret their location as relative to
the location of the configuration file.  Therefore, if you place the
configuration file and the parameter files in the same directory, simply giving
their filenames -- without any path specification -- will be enough.

### Lexicon

Most normalizer components require a *target lexicon.* This is basically a list
of valid word forms that the normalizers can generate, and is used to restrict
the normalizer's output.  A target lexicon requires two files to be given in the
configuration file, under the section `[Lexicon]`:

* `fsmfile=<filename>` is the name of the lexicon file as a finite-state machine
  (see below).

* `symfile=<filename>` is the name of the symbol table belonging to that
  finite-state machine.

The command-line tool `norma_lexicon` can be used to compile or extract these
files.  If your target lexicon is in a UTF8-encoded text file called
`words.txt`, containing one word per line, the following command generates the
files `words.fsm` and `words.sym` that can be used with Norma:

    norma_lexicon -w words.txt -a words.fsm -l words.sym -c

To perform the operation the other way around (i.e., to get back a list of words
in text format from the `.fsm` and `.sym` file), simply replace the `-c` with
the `-x` option.

### Normalizer "Mapper"

The mapper normalizer simply stores a list of "mappings" from non-standard to
standard word forms.  There is only one configuration parameter, under the
section `[Mapper]`:

* `mapfile=<filename>` is the name of the file containing the mappings.

### Normalizer "RuleBased"

The rule-based normalizer works by using context-aware character rewrite rules.
It is described in detail in
[a paper by Bollmann et al. (2011)](http://www.linguistics.rub.de/~bollmann/pub/ranlp11.pdf
"M. Bollmann, F. Petran, & S. Dipper.  2011. Rule-Based Normalization of
Historical Texts.  In Proceedings of RANLP 2011, pp. 32--42.  Hissar,
Bulgaria.").  It expects one parameter under the section `[RuleBased]`:

* `rulesfile=<filename>` is the name of the file containing the rewrite rules.

### Normalizer "WLD"

The WLD normalizer works by computing the weighted Levenshtein distance of a
non-standard word to all possible standard-conforming words, and choosing the
candidate with the lowest distance.  You can specify the following parameters
under the section `[WLD]`:

* `paramfile=<filename>` is the parameter file containing the Levenshtein
  weights.

* `train_ngrams=<number>` specifies how big *n*-grams generated during training
  can be.  The default is 3, i.e., training will generate Levenshtein weights
  for character unigrams, bigrams, and trigrams.

* `train_divisor=<number>` is a divisor for the weights generated during
  training.  It defaults to 7.

* `max_weight=<number>` is the maximum distance between words to consider.  By
  default, it is unlimited, however, we found that memory usage and runtime can
  get excessively high without setting a limit.  As the ideal limit likely
  depends heavily on the individual datasets, it is unclear what a reasonable
  default value could be, so this is a configuration option for now.  (We
  currently use 2.5 for our own data.)
