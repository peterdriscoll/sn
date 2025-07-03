import { Component, OnInit } from '@angular/core';
import { AppStateService } from '../services/app-state.service';

@Component({
  selector: 'app-thread-tabs',
  templateUrl: './thread-tabs.component.html',
  styleUrls: ['./thread-tabs.component.css'],
  standalone: false
})
export class ThreadTabsComponent implements OnInit {
  stepcounts: { threadnum: number, stepcount: number }[] = [];

  constructor(public appState: AppStateService) {}

  ngOnInit(): void {
    this.appState.loadThreadStepCounts();
    this.appState.stepcounts$.subscribe(data => {
      this.stepcounts = data;
    });
  }

  setThread(threadnum: number): void {
    this.appState.threadnum$.next(threadnum);
    this.appState.loadDashboard(); // reload all the relevant data for the new thread
  }
}
