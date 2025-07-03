import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { BehaviorSubject } from 'rxjs';

@Injectable({ providedIn: 'root' })
export class AppStateService {
  private home = 'http://127.0.0.1/';

  threadnum$ = new BehaviorSubject<number>(0);
  stepcount$ = new BehaviorSubject<number>(0);
  stepcounts$ = new BehaviorSubject<any[]>([]);
  currentstepcount$ = new BehaviorSubject<number>(0);
  laststepcount$ = new BehaviorSubject<number>(0);
  running$ = new BehaviorSubject<boolean>(false);
  closing$ = new BehaviorSubject<boolean>(false);

  countcalls$ = new BehaviorSubject<number>(0);
  countframes$ = new BehaviorSubject<number>(0);
  countlogentries$ = new BehaviorSubject<number>(0);
  countcodeentries$ = new BehaviorSubject<number>(0);

  // UI state (not observable for now)
  reruncommand: boolean = false;
  debugstop: any = { name: 'detail', value: '7' };

  // Additional descriptions for status display
  taskdescription: string = '';
  statusdescription: string = '';
  breakpointdescription: string = '';

  constructor(private http: HttpClient) {}

  loadDashboard(): void {
    const url = `${this.home}dashboard.json?threadnum=${this.threadnum$.value}`;
    this.http.get<any>(url).subscribe(response => {
      this.threadnum$.next(response.threadnum);
      this.currentstepcount$.next(response.currentstepcount);
      this.laststepcount$.next(response.laststepcount);
      this.running$.next(response.running);
      this.closing$.next(response.closing);

      this.countcalls$.next(response.countcalls);
      this.countframes$.next(response.countframes);
      this.countlogentries$.next(response.countlogentries);
      this.countcodeentries$.next(response.countcodeentries);
    });
  }

  loadThreadStepCounts(): void {
    const url = `${this.home}stepcount.json`;
    this.http.get<any>(url).subscribe(response => {
      this.stepcounts$.next(response.records);
    });
  }

  submit(action: string, breakpoints: string[], stackdepth: number, debugstop: string): void {
    const url = `${this.home}${action}?threadnum=${this.threadnum$.value}` +
      `&stackdepth=${stackdepth}` +
      `&debugstop=${debugstop}` +
      `&breakpoints=${breakpoints.join(',')}`;
    this.http.get(url).subscribe(() => this.loadDashboard());
  }
}
