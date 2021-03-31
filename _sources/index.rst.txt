Welcome to the DCC++ EX Commandline Interface
=============================================
Running & managing your digital model rail road made easy.

.. epigraph::

   Logic get's you from point A to point B, Imagination get's you wherever you want

         -- Albert Einstein

Summary
-------

As its name implies DccExCli is a terminal application running on a Linux or MacOs terminal or Windows CMD or PowerShell. 
In a nutshell it allows you to interact with your Arduino based Dcc-EX CommandStation. Uploading, managing or operations: All is
possible by different means of connectivity. Besides that we provide a means to describe your layout for automation purposes
allowing to upload e.g. Turnouts to the commandstation and calculating routes across your layout.

What you need to do? Follow some simple instructions and you should be up and running in no time. The Dcc-EX 
team already managed to minimize setting up the hardware, no more wiring (excpet to get the power to your tracks of course). 
Know how to put LEGO together ? Well then you can build the commandstation.

Why are you asking yet another program I have JMRI, Arduino IDE, VisualStudio Code with the PlatformIO extension 
etc. etc. to get things running. 

For most of the time you will need at least the Arduino IDE or the VSC/PIO combo for getting:

1. The commandstation code compiled and loaded onto the Arduino (downloaded from the Dcc-EX website).
2. The serial monitor to interact with the commandstation for either programming or operations.


That all looks very initimidating and even for the seasoned developper it can sometimes become a hassle as well to get 
everything setup and configured. So there should be a solution providing enough freedom for the tinkerers and engineers around 
but most of all some simple tool to get running quickly for conductors - Don't know what those name mean, well head over to the DCC-EX website.

The DCC++ EX Commandline Interface allows to install/manage/configure your DCC++EX commandstation as well as your layout in an oldfashioned 
typewriter way of working. It comes handy but doesn't replace a full UI as a Mobile or Web App you may expect today but its a start towards that
vision and the premises of something much bigger to come. I am not considering here the Throttles used for running your Locos there are quite 
some around with good UX but getting things setup and organized in a simple way.

One goal is also to take out all the technology speak of the user expirience and make all that more palpable for everyone more interested in 
trains rather than digital electronics. Then go buy one of those commercial systems you may say, and yes why not, if you got all the money at 
hand and still want to fight all the tech talk on top of most and be locked into a single vendor to keep things 'simple'.

Due to the fragemented technology environment and providers taking a large degree of freedom implementing exsiting standards the temptation is
to cover functional and technology wise everything. The ongoing engineering in the domain is brilliant everywhere and targeted clearly 
towards supporting everything and everyones ideas. 

The observation, imho, is that currently the community caters mostly to those who want to push everything to the limit and aren't satisified how 
things are done now, want to do it their way etc. basically the engineers and some tinkerers. That way of thinking is absolutley necessary to drive 
inovation etc. but should not inhibit delivery of something easy and straight forward to use even if that implies possibly loosing capabilities not 
used in any case by the vast majority.

Having read this you may be disappointed of what follows indeed the vision is still a long way to go and maybe I am completley wrong 
and everyone in the model rail road community is just happy as things are today. 

Let's try to make things simpler. One step after the other.

More information about CommandStation-EX can be found on `GitHub <https://github.com/DCC-EX/CommandStation-EX>`_ or on the `DCC-EX <https:////dcc-ex.com>`_  website


.. toctree::
   :maxdepth: 4
   :caption: Contents:

   running-cli/index
   layout/index
   cli-commands/index
   roadmap/index
   contributing/index
   download/index
   support/index
   about/index

..
   api/library_root


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
