import { Component, Input, OnChanges, SimpleChanges, ElementRef, ViewChild } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { BreakpointService } from '../services/breakpoint.service';

@Component({
  selector: 'app-world-sets',
  templateUrl: './world-sets.component.html',
  styleUrls: ['./world-sets.component.css'],
  standalone: false
})
export class WorldSetsComponent implements OnChanges {
  @Input() threadnum: number = 0;
  @Input() countworldsets: number = 0;

  @ViewChild('worldsetsid') worldsetsEl!: ElementRef<HTMLDetailsElement>;

  worldsets: any[] = [];

  constructor(
    private http: HttpClient,
    private breakpointService: BreakpointService
  ) {}

  ngOnChanges(changes: SimpleChanges): void {
    if ('countworldsets' in changes && this.countworldsets === 0) {
      this.worldsets = [];
    }
  }

  loadworldsets(opening: boolean): void {
    const field = this.worldsetsEl?.nativeElement;

    if (this.countworldsets === 0) {
      this.worldsets = [];
      return;
    }

    if ((opening && !field?.open) || (!opening && field?.open)) {
      const url = `http://127.0.0.1/worldsets.json?threadnum=${this.threadnum}`;
      this.http.get<any>(url).subscribe(response => {
        this.worldsets = response.records || [];
      });
    }
  }

  setbreakpoint(bp: [string, number]): void {
    this.breakpointService.setbreakpoint(bp);
  }

  breakpointdefaultclass(bp: [string, number], reason: string): string {
    return this.breakpointService.breakpointdefaultclass(bp, reason);
  }
}
