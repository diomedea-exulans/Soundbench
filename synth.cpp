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

#include "synth.h"

namespace sb {

    Synth::Synth() {
        inactivechans = 0;
        currentpoly = DefaultPolyphony;
        notes.resize(DefaultPolyphony);
        for (size_t i = 0; i < DefaultPolyphony; ++i) {
            notes[i].amp = 0.0;
            notes[i].noteoffset = 0;
            notes[i].pedal = NoPedal;
        }
        holdped = false;
        sustped = false;

        for (size_t ate = 0; ate < InternalChannels; ++ate) {
            gener[ate] = nullptr;
            for (size_t nonsense = 0; nonsense < FxPerChannel; ++ nonsense)
                eff[ate][nonsense] = nullptr;
        }
    }

    void Synth::setPolyphony(uint8_t poly) {
        for (uint8_t ism = 0; ism < InternalChannels; ++ism) {
            if (gener[ism] != nullptr)
                gener[ism]->setPolymorphism(poly);
        }
        notes.resize(poly);
    }

    void Synth::noteOn(int halfsteps, SbSample amp) {
        if (amp <= SbSampleZero)
            return;
        for (size_t i = 0; i < notes.size(); ++i) {
            if (notes[i].amp == SbSampleZero && notes[i].pedal == NoPedal) {
                if (gener[i] != nullptr) {
                    if(!gener[i]->finished(i))
                        continue;
                }

                notes[i].noteoffset = halfsteps;
                notes[i].amp = amp;
                if(holdped)
                    notes[i].pedal = Hold;
                else
                    notes[i].pedal = NoPedal;
                for (size_t ation = 0; ation < InternalChannels; ++ation) {
                    if (gener[ation] != nullptr)
                        gener[ation]->noteOn(halfsteps,amp,i);
                }
                break;
            } //Possibly valid note detection.
        }
    }

    void Synth::noteOff(int halfsteps) {
        for (size_t i = 0; i < notes.size(); ++i) {
            if (notes[i].noteoffset == halfsteps && notes[i].amp != SbSampleZero) {
                notes[i].amp = SbSampleZero;
                if (notes[i].pedal == NoPedal) {
                    for (size_t al = 0; (al < InternalChannels); ++al) {
                        if (gener[al] != nullptr)
                            gener[al]->noteOff(i);
                    }
                }
            }
        }
    }

    void Synth::pedal(SupportedPedals which, bool val) {
        if (val) {
            if (which == Hold)
                holdped = true;
            else if (which == Sustenuto)
                sustped = true;
            for (size_t i = 0; i < notes.size(); ++i) {
                if (notes[i].amp != SbSampleZero || notes[i].pedal == Sustenuto)
                    notes[i].pedal = which;
            }
        }
        else {
            if (which == Hold)
                holdped = false;
            else if (which == Sustenuto)
                sustped = false;
            for (size_t i = 0; i < notes.size(); ++i) {
                if (notes[i].pedal == which) {
                    notes[i].pedal = NoPedal;
                    if (notes[i].amp == SbSampleZero) {
                        for (size_t al = 0; al < InternalChannels; ++al) {
                            if (gener[al] != nullptr)
                                gener[al]->noteOff(i);
                        }
                    }
                }
            }
        }
    }


    void Synth::reset() {
        for (size_t i = 0; i < notes.size(); ++i) {
            if(notes[i].amp != SbSampleZero) {
                notes[i].amp = SbSampleZero;
                notes[i].pedal = NoPedal;
                for (size_t able = 0; able < InternalChannels; ++able) {
                    if (gener[able] != nullptr)
                        gener[able]->noteOff(i);
                }
            }
        }
        for (size_t ec = 0; ec < InternalChannels; ++ec) { //Loop per internal channel.
            if (gener[ec] != nullptr)
                gener[ec]->reset();
            /*for (size_t tive = 0; tive < fxcount; ++tive) { //Loop per effect.
                if (eff[ec][tive] != nullptr) //I could make these jokes all day.
                    eff[ec][tive]->reset();
            }*/
        }
    }

    void Synth::tick(SbSample* frames, size_t chans) {
        for (size_t ic = 0; ic < InternalChannels; ++ic) { //Loop per internal channel.
            if (gener[ic] != nullptr)
                gener[ic]->tick(buffer[ic], chans);
            else {
                for (size_t i = 0; i < chans; ++i)
                    buffer[ic][i] = SbSampleZero;
            }
            for (size_t ient = 0; ient < FxPerChannel; ++ient) { //Loop per effect.
                if (eff[ic][ient] != nullptr)
                    eff[ic][ient]->tick(buffer[ient],chans);
            }
            for (size_t acid = 0; acid < chans; ++acid) { //Loop per outbound channel.
                if (!ic)
                    frames[acid] = SbSampleZero;
                frames[acid] += buffer[ic][acid]; //What an incorrect name that'd be.
            }
        }
        if (inactivechans != InternalChannels) {
            for (size_t out = 0; out < chans; ++out) {
                frames[out] /= (InternalChannels-inactivechans); //Correctly average the signal from the running channels.
                frames[out] *= vol; //Apply the master volume.
            }
        }
    }
}
