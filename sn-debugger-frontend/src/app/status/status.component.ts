import { Component, OnInit } from '@angular/core';
import { AppStateService } from '../services/app-state.service';

@Component({
  selector: 'app-status',
  templateUrl: './status.component.html',
  standalone: false
})
export class StatusComponent implements OnInit {
  constructor(public appState: AppStateService) {}

  ngOnInit(): void {
    this.appState.loadDashboard();
  }

  get threadnum(): number {
    return this.appState.threadnum$.value;
  }

  get currentstepcount(): number {
    return this.appState.currentstepcount$.value;
  }

  get laststepcount(): number {
    return this.appState.laststepcount$.value;
  }

  get running(): boolean {
    return this.appState.running$.value;
  }

  get closing(): boolean {
    return this.appState.closing$.value;
  }

  get taskdescription(): string {
    return this.appState.taskdescription;
  }

  get statusdescription(): string {
    return this.appState.statusdescription;
  }

  get breakpointdescription(): string {
    return this.appState.breakpointdescription;
  }
}
