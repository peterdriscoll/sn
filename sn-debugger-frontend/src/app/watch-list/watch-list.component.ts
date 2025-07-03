import { Component, ElementRef, OnDestroy, OnInit, ViewChild } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { DomSanitizer, SafeHtml } from '@angular/platform-browser';
import { AppStateService } from '../services/app-state.service';
import { Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';

@Component({
  selector: 'app-watch-list',
  templateUrl: './watch-list.component.html',
  standalone: false
})
export class WatchListComponent implements OnInit, OnDestroy {
  @ViewChild('watchlistid') watchlistEl!: ElementRef<HTMLDetailsElement>;

  threadnum: number = 0;
  watchlist: any[] = [];
  watchListDisplayAll: boolean = true;
  watchList: Set<string> = new Set();

  private destroy$ = new Subject<void>();

  constructor(
    private http: HttpClient,
    private sanitizer: DomSanitizer,
    private appState: AppStateService
  ) {}

  ngOnInit(): void {
    this.appState.threadnum$
      .pipe(takeUntil(this.destroy$))
      .subscribe(threadnum => {
        this.threadnum = threadnum;
        this.loadwatchlist(false);
      });
  }

  ngOnDestroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
  }

  trustAsHtml(text: string): SafeHtml {
    return this.sanitizer.bypassSecurityTrustHtml(text);
  }

  loadwatchlist(opening: boolean): void {
    const field = this.watchlistEl?.nativeElement;
    if ((opening && !field?.open) || (!opening && field?.open)) {
      const url = `http://127.0.0.1/watchlist.json?threadnum=${this.threadnum}`;
      this.http.get<any>(url).subscribe(response => {
        this.watchlist = response.records || [];
      });
    }
  }

  toggleWatchListDisplayAll(): void {
    this.watchListDisplayAll = !this.watchListDisplayAll;
  }

  setWatch(id: string): void {
    if (this.watchList.has(id)) {
      this.watchList.delete(id);
    } else {
      this.watchList.add(id);
    }
  }

  getWatch(id: string): boolean {
    return this.watchList.has(id);
  }

  getWatchDisplay(id: string): boolean {
    return this.watchListDisplayAll || this.watchList.has(id);
  }
}
