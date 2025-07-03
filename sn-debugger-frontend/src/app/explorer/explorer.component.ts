import { Component } from '@angular/core';
import { BreakpointService } from '../services/breakpoint.service';

@Component({
  selector: 'app-explorer',
  templateUrl: './explorer.component.html',
  styleUrls: ['./explorer.component.css'],
  standalone: false
})
export class ExplorerComponent {
  constructor(private breakpointService: BreakpointService) {}

  get breakpoint(): [string, number] | undefined {
    return this.breakpointService.currentBreakpoint;
  }

  setBreakpoint(bp: [string, number]): void {
    this.breakpointService.setbreakpoint(bp);
  }

  getBreakpointClass(bp: [string, number]): string {
    return this.breakpointService.breakpointclass(bp);
  }

  getBreakpointDefaultClass(bp: [string, number], fallback: string): string {
    return this.breakpointService.breakpointdefaultclass(bp, fallback);
  }
}
