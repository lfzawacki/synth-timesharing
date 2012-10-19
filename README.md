Monophonic Synthesizer Timesharing (synth-timesharing)
=================

Original author: Marcelo Oliveira Johann
Modifications: Lucas Fialho Zawacki

This project is aimed at developing a software plugin that enables
the use of a monophonic synthesizer as if it had support for poliphony.

This is made possible by using a techinique we call "timesharing" of
the notes. What's required to make it work is:

* A monophonic synthesizer with MIDI input and audio output
* A midi input method (e.g. software or hardware keyboard interface)
* A sound output method (e.g. your speakers or tape or recording)
* A DAW with the synth-timesharing plugin installed

It's currently a VST plugin, but I have plans of porting it to lv2 and maybe core-audio if that's doable, based on my disponibility.

## How does it work?

The musician plays the synth via the MIDI interface and the synth-timesharing plugin monitors it for notes and chords and communicates them to the actual synth. When a chord is played the plugin actually performs a very quick sequence of the notes composing this chord (known musically as an arpeggio), but what will be actually heard will be the sound of the notes played simultaneously. This is done by recording the sound of each note and mixing them together in software to generate the final result. This is done on demand for each new poliphony.

[A better explanation with images (SOON!)](#)

## Download

Currently you can download a Windows .dll implementation of the VST plugin. Soon I intend to make the Mac version available.

## Setting it up

The plugin requires a bit of work on your part to setup the different tracks in your favorite Digital Audio Workstation. Here I try to explain the setup for different DAWs.

### Soon!