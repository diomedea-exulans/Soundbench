/*
    This file is part of Soundbench.

    Soundbench is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Soundbench is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Soundbench.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2012  Amaya S.
*/

#include "soundbenchmain.h"

void SoundbenchMain::importOpen() {
    stopAndReset();
    QString chosenfile = QFileDialog::getOpenFileName(
                NULL,"Choose a file to import...", "~/", "Standard MIDI Files (*.mid *.smf);;All Files (*)");

    if (!chosenfile.size()) {
        em->start();
        return;
    }
    plai->setFile(chosenfile.toStdString());
    if(!plai->loadFile()) {
        em->start();
        return;
    }
    ui->exportButton->setEnabled(true);
    ui->songsTracksList->setCurrentRow(0);

    ui->tempoBox->setValue(plai->getTempo());
    if (plai->getTempo()) {
        ui->tempoBox->setEnabled(true);
        ui->tempoBox->setToolTip("Change the starting tempo of the current track.");
    }
    else {
        ui->tempoBox->setDisabled(true);
        ui->tempoBox->setToolTip("This MIDI file does not represent time in terms of beats, and thus the tempo box has been disabled.");
    }

    em->start();
}

void SoundbenchMain::exportOpen() {
    if(plai->empty())
        return;
    stopAndReset();
    met->setProgress(0);
    QString chosenfile = QFileDialog::getSaveFileName(
                NULL,"Choose a file to export to...", "~/",
                "Wave File (*.wav);;AIFF File (*.aiff);;Free Lossless Audio Codec File (*.flac);;Headerless RAW file (*.raw);;MAT5 Binary Data File (*.mat)"); //Put .mid back in here in version 0.3.0.
    if (!chosenfile.size()) {
        em->start();
        met->startMeter();
        return;
    }

    plai->setFile(chosenfile.toStdString());
    plai->writeFile();
    syn->reset();
    met->startMeter();
    em->start();
}

void SoundbenchMain::setTempo(int tiem) {
    plai->setTempo(tiem);
    plai->loadTrack(ui->songsTracksList->currentRow());
}

void SoundbenchMain::testSynth(bool b) {
    if (b)
        syn->noteOn(0,1.0);
    else
        syn->noteOff(0);
}

void SoundbenchMain::playSynth(bool b) {
    if (plai->empty())
        return;
    if (b)
        plai->startPlay();
    else
        plai->stopPlay();
}

void SoundbenchMain::silence() {
    stopAndReset();
    em->start();
}

void SoundbenchMain::setMasterVolume(int) {
    syn->volume() = static_cast<SbSample>(ui->volumeSlider->value())/ui->volumeSlider->maximum();
}

void SoundbenchMain::setSampleRate(int which) {
    ui->playButton->setChecked(false);
    em->setSamplingRate(sb::SupportedRates[which]);
    arch->buildSynth(syn,blu);
    em->start();
}

void SoundbenchMain::setGenType(int which) {
    switch (which) {
    case 0:
        blu->gener[which] = static_cast<sb::GenerType>(ui->gener1TypeBox->currentIndex());
        if (blu->gener[which] != sb::NoGener)
            ui->gen1SettButton->setEnabled(true);
        else
            ui->gen1SettButton->setDisabled(true);
        break;
    case 1:
        blu->gener[which] = static_cast<sb::GenerType>(ui->gener2TypeBox->currentIndex());
        if (blu->gener[which] != sb::NoGener)
            ui->gen2SettButton->setEnabled(true);
        else
            ui->gen2SettButton->setDisabled(true);
        break;
    case 3:
        blu->gener[which] = static_cast<sb::GenerType>(ui->gener3TypeBox->currentIndex());
        if (blu->gener[which] != sb::NoGener)
            ui->gen3SettButton->setEnabled(true);
        else
            ui->gen3SettButton->setDisabled(true);
        break;
    case 4:
        blu->gener[which] = static_cast<sb::GenerType>(ui->gener4TypeBox->currentIndex());
        if (blu->gener[which] != sb::NoGener)
            ui->gen4SettButton->setEnabled(true);
        else
            ui->gen4SettButton->setDisabled(true);
        break;
    default:
        std::cerr << "A generator type-changing function was called by some random widget.\n";
        return;
    }
    stopAndReset();
    arch->buildSynth(syn,blu);
    em->start();
}

void SoundbenchMain::setGenSett(int which) {
    stopAndReset();
    met->pauseMeter();
    genSetts(which);
    arch->buildSynth(syn,blu);
    syn->reset();
    met->startMeter();
    em->start();
}

void SoundbenchMain::setTrack(int e) {
    if(!plai->loadTrack(static_cast<uint16_t>(e)))
        return;
    ui->tempoBox->setValue(plai->getTempo());
    met->startMeter();
}

void SoundbenchMain::setPoly(int tracks) {
    stopAndReset();
    syn->setPolyphony(tracks);
    em->start();
}
