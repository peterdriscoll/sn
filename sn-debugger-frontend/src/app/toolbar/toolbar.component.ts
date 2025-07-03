import { Component, Output, EventEmitter, ElementRef, ViewChild } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { AppStateService } from '../services/app-state.service';

@Component({
  selector: 'app-toolbar',
  templateUrl: './toolbar.component.html',
  standalone: false
})
export class ToolbarComponent {
  @Output() commandSubmit = new EventEmitter<string>();
  @ViewChild('stepcountid') stepcountInput!: ElementRef;

  debugstopoptions = [
    { name: 'none', value: '0' },
    { name: 'end', value: '1' },
    { name: 'task', value: '2' },
    { name: 'error', value: '3' },
    { name: 'warning', value: '4' },
    { name: 'info', value: '5' },
    { name: 'debug', value: '6' },
    { name: 'detail', value: '7' }
  ];

  constructor(
    private http: HttpClient,
    public appState: AppStateService
  ) {}

  // Getters/setters for binding
  get stepcount() {
    return this.appState.stepcount$.value;
  }
  set stepcount(val: number) {
    this.appState.stepcount$.next(val);
    this.updatereruncommand();
  }

  get reruncommand() {
    return this.appState.reruncommand;
  }

  get debugstop() {
    return this.appState.debugstop;
  }
  set debugstop(value: any) {
    this.appState.debugstop = value;
  }

  submit(command: string) {
    this.commandSubmit.emit(command);
  }

  gotostepcountsetfocus() {
    this.stepcountInput?.nativeElement.focus();
  }

  settingssetfocus() {
    this.settings();
  }

  gotostepcount() {
    const threadnum = this.appState.threadnum$.value;
    const stepcount = this.appState.stepcount$.value;
    this.http.get(`http://127.0.0.1/gotostepcountjs?threadnum=${threadnum}&stepcount=${stepcount}`)
      .subscribe(() => {
        this.submit('loaddata');
      });
  }

  updatereruncommand() {
    const current = this.appState.currentstepcount$.value;
    const last = this.appState.laststepcount$.value;
    const step = this.appState.stepcount$.value;
    this.appState.reruncommand = last < step && step < current;
  }

  reruntostackpoint(stepcount: number) {
    this.stepcount = stepcount;
    setTimeout(() => {
      const el = this.stepcountInput?.nativeElement;
      if (el) {
        el.value = stepcount;
        el.focus();
        el.scrollIntoView({ behavior: "smooth", block: "end", inline: "nearest" });
      }
    });
  }

  settings() {
    this.submit('loaddata');
  }
}
